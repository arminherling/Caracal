#include "LLVMCodeGenerator.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/ADT/APFloat.h>
#include <Caracal/Syntax/GroupingExpression.h>
#include <Caracal/Syntax/TypeFieldDeclaration.h>

namespace Caracal
{
    constexpr auto GlobalInitFunctionName = "__caracal_global_init";

    [[nodiscard]] static auto InitializeTypeToLLVMType(llvm::LLVMContext& context) noexcept
    {
        return std::unordered_map<Type, llvm::Type*>{
            { Type::Void(), llvm::Type::getVoidTy(context) },
            { Type::Bool(), llvm::Type::getInt1Ty(context) },
            { Type::U8(), llvm::Type::getInt8Ty(context) },
            { Type::I32(), llvm::Type::getInt32Ty(context) },
            { Type::F32(), llvm::Type::getFloatTy(context) },
            { Type::String(), llvm::PointerType::getUnqual(context) },
        };
    }

    [[nodiscard]] static llvm::Type* GetLLVMTypeForCaraType(Type type, llvm::LLVMContext& context) noexcept
    {
        if (type.isReference())
        {
            return llvm::PointerType::getUnqual(context);
        }

        const auto llvmTypes = InitializeTypeToLLVMType(context);
        if (const auto result = llvmTypes.find(type); result != llvmTypes.end())
            return result->second;

        return nullptr;
    }

    [[nodiscard]] static llvm::Type* GetLLVMTypeForCaraType(
        Type type,
        llvm::LLVMContext& context,
        llvm::Module& llvmModule,
        SemanticContext& semanticContext) noexcept
    {
        if (auto* llvmType = GetLLVMTypeForCaraType(type, context))
            return llvmType;

        if (type.kind() == TypeKind::Type)
        {
            const auto typeName = semanticContext.getNameByType(type);
            if (!typeName.empty())
            {
                if (auto* structType = llvm::StructType::getTypeByName(context, typeName))
                    return structType;
            }
        }

        return nullptr;
    }

    [[nodiscard]] static auto InitializeBuiltinFunctions() noexcept
    {
        return std::unordered_map<std::string_view, std::string_view>{
            { std::string_view("print"), std::string_view("printf") },
        };
    }

    [[nodiscard]] static llvm::Value* PromoteVariadicArgumentIfNeeded(
        llvm::Value* llvmArgumentValue,
        llvm::IRBuilderBase* irBuilder,
        llvm::Module& llvmModule,
        bool functionIsVariadic) noexcept
    {
        if (!functionIsVariadic || llvmArgumentValue == nullptr)
            return llvmArgumentValue;

        if (llvmArgumentValue->getType()->isIntegerTy(1) || llvmArgumentValue->getType()->isIntegerTy(8))
        {
            return irBuilder->CreateZExt(llvmArgumentValue, llvm::Type::getInt32Ty(llvmModule.getContext()));
        }
        else if (llvmArgumentValue->getType()->isFloatTy())
        {
            return irBuilder->CreateFPExt(llvmArgumentValue, llvm::Type::getDoubleTy(llvmModule.getContext()));
        }

        return llvmArgumentValue;
    }

    [[nodiscard]] static void SetupFunctionParameters(
        const FunctionDefinition& functionDefinition,
        llvm::Function* llvmFunction,
        LLVMScope* scope,
        llvm::IRBuilderBase* irBuilderBase) noexcept
    {
        const auto& functionParameters = functionDefinition.parameters();
        auto functionArguments = llvmFunction->args();
        for (auto& argument : functionArguments)
        {
            const auto& parameter = functionParameters.at(argument.getArgNo());
            argument.setName(parameter.name());

            if (parameter.type().isReference())
            {
                // we can mark references as nonnull
                llvmFunction->addParamAttr(
                    argument.getArgNo(), 
                    llvm::Attribute::get(llvmFunction->getContext(), llvm::Attribute::AttrKind::NonNull));

                // references dont need alloca, we can just use the argument pointer directly as the storage
                scope->addVariableBinding(parameter.name(), &argument);
            }
            else
            {
                scope->addVariableBinding(parameter.name(), &argument);
            }
        }
    }

    LLVMCodeGenerator::LLVMCodeGenerator(
        SemanticContext& semanticContext,
        llvm::Module& llvmModule)
        : m_semanticContext{ semanticContext }
        , m_currentType{ Type::Undefined() }
        , m_llvmModule{ llvmModule }
        , m_currentFunction{ nullptr }
        , m_currentConditionBlock{ nullptr }
        , m_currentEndBlock{ nullptr }
        , m_irBuilder{ std::make_unique<llvm::IRBuilder<>>(llvmModule.getContext()) }
    {
        m_scopes.emplace_back(std::make_unique<LLVMScope>(nullptr));
    }

    bool LLVMCodeGenerator::generate()
    {
        generateTopLevelDeclarations();
        generateFunctionBodies();

        return true;
    }

    void LLVMCodeGenerator::generateNode(Node* node) noexcept
    {
        switch (node->kind())
        {
            case NodeKind::ConstantDeclaration:
            {
                generateConstantDeclaration(static_cast<ConstantDeclaration*>(node));
                break;
            }
            case NodeKind::VariableDeclaration:
            {
                generateVariableDeclaration(static_cast<VariableDeclaration*>(node));
                break;
            }
            case NodeKind::AssignmentStatement:
            {
                generateAssignmentStatement(static_cast<AssignmentStatement*>(node));
                break;
            }
            case NodeKind::FunctionDefinitionStatement:
            {
                auto functionDefinitionNode = static_cast<FunctionDefinitionStatement*>(node);
                if (!functionDefinitionNode->isExtern())
                {
                    generateFunctionDefinition(functionDefinitionNode);
                }

                break;
            }
            case NodeKind::EnumDefinitionStatement:
            {
                break;
            }
            case NodeKind::TypeDefinitionStatement:
            {
                generateTypeDefinition(static_cast<TypeDefinitionStatement*>(node));
                break;
            }
            case NodeKind::IfStatement:
            {
                generateIfStatement(static_cast<IfStatement*>(node));
                break;
            }
            case NodeKind::WhileStatement:
            {
                generateWhileStatement(static_cast<WhileStatement*>(node));
                break;
            }
            case NodeKind::BreakStatement:
            {
                generateBreakStatement();
                break;
            }
            case NodeKind::SkipStatement:
            {
                generateSkipStatement();
                break;
            }
            case NodeKind::ExpressionStatement:
            {
                generateExpressionStatement(static_cast<ExpressionStatement*>(node));
                break;
            }
            case NodeKind::ReturnStatement:
            {
                generateReturnStatement(static_cast<ReturnStatement*>(node));
                break;
            }
            case NodeKind::BlockNode:
            {
                generateBlockNode(static_cast<BlockNode*>(node));
                break;
            }
            default:
            {
                TODO("Missing NodeKind!!");
                break;
            }
        }
    }

    void LLVMCodeGenerator::generateConstantDeclaration(ConstantDeclaration* node) noexcept
    {
        const auto leftExpression = node->leftExpression().get();
        if (leftExpression->kind() != NodeKind::NameExpression)
        {
            TODO("Left expression of constant declaration must be a name expression");
        }
        const auto nameExpression = static_cast<NameExpression*>(leftExpression);
        const auto& name = nameExpression->name();

        auto* rightExpression = node->rightExpression().get();
        if (node->isGlobalConstant() && rightExpression->kind() == NodeKind::BinaryExpression)
        {
            auto* binaryExpression = static_cast<BinaryExpression*>(rightExpression);
            if (binaryExpression->binaryOperator() == BinaryOperatorKind::ConstructorCall)
            {
                if (tryGenerateGlobalConstructorCall(name, binaryExpression))
                {
                    return;
                }
            }
        }
        
        auto llvmValue = generateExpression(rightExpression);
        if (!llvmValue)
        {
            TODO("Right-hand side of constant declaration produced null during codegen");
            return;
        }
        auto isGlobalConstant = node->isGlobalConstant();

        if (isGlobalConstant)
        {
            if (auto llvmConstant = llvm::dyn_cast<llvm::Constant>(llvmValue))
            {
                createGlobalValue(name, llvmConstant, true);
            }
            else
            {
                TODO("Global constant must be an llvm::Constant");
            }
        }
        else
        {
            const auto llvmType = llvmValue->getType();
            const auto localValue = createLocalValue(name, llvmType);
            
            m_irBuilder->CreateStore(llvmValue, localValue);
        }
    }

    void LLVMCodeGenerator::generateVariableDeclaration(VariableDeclaration* node) noexcept
    {
        const auto leftExpression = node->leftExpression().get();
        if (leftExpression->kind() != NodeKind::NameExpression)
        {
            TODO("Left expression of variable declaration must be a name expression");
        }
        const auto nameExpression = static_cast<NameExpression*>(leftExpression);
        const auto& name = nameExpression->name();

        const auto* rightExpression = node->rightExpression().get();
        if (rightExpression->kind() == NodeKind::BinaryExpression)
        {
            const auto* binaryExpression = static_cast<const BinaryExpression*>(rightExpression);
            if (binaryExpression->binaryOperator() == BinaryOperatorKind::ConstructorCall)
            {
                auto* llvmType = GetLLVMTypeForCaraType(binaryExpression->type(), m_llvmModule.getContext(), m_llvmModule, m_semanticContext);
                auto* localValue = createLocalValue(name, llvmType);
                if (tryGenerateConstructorCallInto(binaryExpression, localValue))
                {
                    return;
                }
            }
        }

        auto llvmValue = generateExpression(rightExpression);
        if (!llvmValue)
        {
            TODO("Right-hand side of variable declaration produced null during codegen");
            return;
        }

        const auto llvmType = llvmValue->getType();
        const auto localValue = createLocalValue(name, llvmType);

        m_irBuilder->CreateStore(llvmValue, localValue);
    }

    void LLVMCodeGenerator::generateExpressionStatement(ExpressionStatement* node) noexcept
    {
        const auto expression = node->expression().get();
        static_cast<void>(generateExpression(expression));
    }

    void LLVMCodeGenerator::generateAssignmentStatement(AssignmentStatement* node) noexcept
    {
        const auto leftExpression = node->leftExpression().get();
        const auto rightExpression = node->rightExpression().get();

        if (rightExpression->kind() == NodeKind::BinaryExpression)
        {
            auto* binaryExpression = static_cast<BinaryExpression*>(rightExpression);
            if (binaryExpression->binaryOperator() == BinaryOperatorKind::ConstructorCall)
            {
                auto llvmLeftValue = generateExpression(leftExpression);
                if (llvmLeftValue == nullptr)
                {
                    TODO("Left expression return nullptr");
                    return;
                }

                llvm::Value* destinationPtr = nullptr;
                if (leftExpression->type().isReference())
                {
                    destinationPtr = getPointerForAssignment(leftExpression, llvmLeftValue);
                }
                else if (auto localLoad = llvm::dyn_cast<llvm::LoadInst>(llvmLeftValue))
                {
                    destinationPtr = localLoad->getPointerOperand();
                }

                if (destinationPtr == nullptr)
                {
                    TODO("Left expression of assignment statement must be assignable");
                    return;
                }

                if (tryGenerateConstructorCallInto(binaryExpression, destinationPtr))
                {
                    return;
                }
            }
        }

        auto llvmLeftValue = generateExpression(leftExpression);
        if (llvmLeftValue == nullptr)
        {
            TODO("Left expression return nullptr");
            return;
        }
        auto llvmRightValue = generateExpression(rightExpression);
        if (llvmRightValue == nullptr)
        {
            TODO("Right expression return nullptr");
            return;
        }

        const bool leftIsReference = leftExpression->type().isReference();
        const bool rightIsReference = rightExpression->type().isReference();

        llvm::Value* valueToStore = llvmRightValue;
        llvm::Value* destinationPtr = nullptr;

        // if left is a ref we can directly store into the pointer
        if (leftIsReference)
        {
            destinationPtr = getPointerForAssignment(leftExpression, llvmLeftValue);
            m_irBuilder->CreateStore(valueToStore, destinationPtr);
            return;
        }

        // If right is ref we need to load the pointee value then store into left's storage
        if (rightIsReference)
        {
            auto rightValCarType = rightExpression->type().toValue();
            auto llvmRightValType = GetLLVMTypeForCaraType(rightValCarType, m_llvmModule.getContext());

            valueToStore = dereferenceIfNeeded(rightExpression, llvmRightValue, llvmRightValType);
        }

        if(auto localLoad = (llvmLeftValue ? llvm::dyn_cast<llvm::LoadInst>(llvmLeftValue) : nullptr))
        {
            m_irBuilder->CreateStore(valueToStore, localLoad->getPointerOperand());
        }
        else
        {
            TODO("Left expression of assignment statement must be a load instruction");
        }
    }

    void LLVMCodeGenerator::generateFunctionDefinition(FunctionDefinitionStatement* node) noexcept
    {
        auto functionType = node->type();
        const auto& body = node->bodyNode();

        generateFunction(functionType, body.get());
    }

    void LLVMCodeGenerator::generateFunction(Type functionType, BlockNode* body) noexcept
    {
        auto& functionDefinition = m_semanticContext.getFunctionDefinition(functionType);
        auto& functionName = functionDefinition.fullName();

        m_currentFunction = getFunctionDeclaration(functionDefinition);

        pushScope();

        auto entry = llvm::BasicBlock::Create(m_llvmModule.getContext(), "entry", m_currentFunction);
        m_irBuilder->SetInsertPoint(entry);

        SetupFunctionParameters(functionDefinition, m_currentFunction, currentScope(), m_irBuilder.get());

        generateBlockNode(body);

        auto llvmReturnType = m_currentFunction->getReturnType();
        if (llvmReturnType->isVoidTy() && !m_irBuilder->GetInsertBlock()->getTerminator())
        {
            // we need to add a return void if there isnt one yet
            m_irBuilder->CreateRetVoid();
        }
        
        popScope();
        m_currentFunction = nullptr;
    }

    void LLVMCodeGenerator::generateSynthesizedConstructor(const FunctionDefinition& functionDefinition) noexcept
    {
        auto& typeDefinition = m_semanticContext.getTypeDefinition(functionDefinition.parentType());

        m_currentFunction = getFunctionDeclaration(functionDefinition);

        pushScope();

        auto* entry = llvm::BasicBlock::Create(m_llvmModule.getContext(), "entry", m_currentFunction);
        m_irBuilder->SetInsertPoint(entry);

        SetupFunctionParameters(functionDefinition, m_currentFunction, currentScope(), m_irBuilder.get());

        auto* thisValue = currentScope()->getVariableBinding("this");
        if (thisValue == nullptr)
        {
            TODO("Synthesized constructor requires this parameter");
            popScope();
            m_currentFunction = nullptr;
            return;
        }

        auto* thisPointer = getPointerForAssignment(nullptr, thisValue);
        if (thisPointer == nullptr)
        {
            TODO("Could not get this pointer for synthesized constructor");
            popScope();
            m_currentFunction = nullptr;
            return;
        }

        for (const auto& fieldDefinition : typeDefinition.fields())
        {
            const auto& fieldName = fieldDefinition.name();
            auto* fieldPointer = getPointerToField(thisPointer, functionDefinition.parentType(), fieldName);
            if (fieldPointer == nullptr)
            {
                TODO("Could not get pointer to field in synthesized constructor");
                continue;
            }

            llvm::Value* fieldValue = nullptr;
            if (auto* parameterValue = currentScope()->getVariableBinding(fieldName))
            {
                fieldValue = parameterValue;
            }
            else if (fieldDefinition.expression() != nullptr)
            {
                fieldValue = generateExpression(fieldDefinition.expression());
            }

            if (fieldValue != nullptr)
            {
                auto* llvmFieldType = GetLLVMTypeForCaraType(fieldDefinition.type(), m_llvmModule.getContext(), m_llvmModule, m_semanticContext);
                fieldValue = dereferenceIfNeeded(fieldDefinition.expression(), fieldValue, llvmFieldType);
                m_irBuilder->CreateStore(fieldValue, fieldPointer);
            }
        }

        if (!m_irBuilder->GetInsertBlock()->getTerminator())
        {
            m_irBuilder->CreateRetVoid();
        }

        popScope();
        m_currentFunction = nullptr;
    }

    llvm::Function* LLVMCodeGenerator::getFunctionDeclaration(const FunctionDefinition& functionDefinition)
    {
        auto& functionName = functionDefinition.fullName();
        auto llvmFunction = m_llvmModule.getFunction(functionName);
        if (llvmFunction != nullptr)
            return llvmFunction;

        llvmFunction = m_llvmModule.getFunction(functionName);
        if (llvmFunction != nullptr)
            return llvmFunction;

        TODO("This shouldn't happen");
        return nullptr;
    }

    void LLVMCodeGenerator::generateTypeDefinition(TypeDefinitionStatement* node) noexcept
    {
        auto thisType = node->type();
        auto typeDefinition = m_semanticContext.getTypeDefinition(thisType);

        const auto& statements = node->bodyNode()->statements();
        for(const auto& statement : statements)
        {
            if(statement->kind() == NodeKind::MethodDefinitionStatement)
            {
                auto method = static_cast<MethodDefinitionStatement*>(statement.get());
                auto methodType = method->type();
                auto methodBody = method->bodyNode().get();
                generateFunction(methodType, methodBody);
            }
        }
    }

    void LLVMCodeGenerator::generateIfStatement(IfStatement* node) noexcept
    {
        const auto hasFalseBlock = node->hasFalseBlock();
        const auto condition = node->condition().get();
        auto llvmCondition = generateExpression(condition);

        auto trueBlock = llvm::BasicBlock::Create(m_llvmModule.getContext(), "if_true", m_currentFunction);
        llvm::BasicBlock* falseBlock = nullptr;
        if (hasFalseBlock)
        {
            falseBlock = llvm::BasicBlock::Create(m_llvmModule.getContext(), "if_false");
        }
        auto afterBlock = llvm::BasicBlock::Create(m_llvmModule.getContext(), "if_end");

        if(hasFalseBlock)
        {
            m_irBuilder->CreateCondBr(llvmCondition, trueBlock, falseBlock);
        }
        else
        {
            m_irBuilder->CreateCondBr(llvmCondition, trueBlock, afterBlock);
        }

        m_irBuilder->SetInsertPoint(trueBlock);
        generateNode(node->trueStatement().get());
        if (!m_irBuilder->GetInsertBlock()->getTerminator())
        {
            m_irBuilder->CreateBr(afterBlock);
        }

        trueBlock = m_irBuilder->GetInsertBlock();

        if(hasFalseBlock)
        {
            m_currentFunction->insert(m_currentFunction->end(), falseBlock);
            m_irBuilder->SetInsertPoint(falseBlock);
            generateNode(node->falseStatement().value().get());
            if (!m_irBuilder->GetInsertBlock()->getTerminator())
            {
                m_irBuilder->CreateBr(afterBlock);
            }

            falseBlock = m_irBuilder->GetInsertBlock();
        }

        m_currentFunction->insert(m_currentFunction->end(), afterBlock);
        m_irBuilder->SetInsertPoint(afterBlock);
    }

    void LLVMCodeGenerator::generateWhileStatement(WhileStatement* node) noexcept
    {
        const auto condition = node->condition().get();
        auto conditionBlock = llvm::BasicBlock::Create(m_llvmModule.getContext(), "while_condition", m_currentFunction);
        
        m_irBuilder->CreateBr(conditionBlock);
        m_irBuilder->SetInsertPoint(conditionBlock);
        auto llvmCondition = generateExpression(condition);
        auto oldConditionBlock = m_currentConditionBlock;
        m_currentConditionBlock = conditionBlock;

        auto loopBlock = llvm::BasicBlock::Create(m_llvmModule.getContext(), "while_loop");
        auto afterBlock = llvm::BasicBlock::Create(m_llvmModule.getContext(), "while_end");
        auto oldAfterBlock = m_currentEndBlock;
        m_currentEndBlock = afterBlock;

        m_irBuilder->CreateCondBr(llvmCondition, loopBlock, afterBlock);

        m_currentFunction->insert(m_currentFunction->end(), loopBlock);
        m_irBuilder->SetInsertPoint(loopBlock);
        generateNode(node->trueStatement().get());
        if (!m_irBuilder->GetInsertBlock()->getTerminator())
        {
            m_irBuilder->CreateBr(conditionBlock);
        }

        // restore blocks
        m_currentConditionBlock = oldConditionBlock;
        m_currentEndBlock = oldAfterBlock;

        m_currentFunction->insert(m_currentFunction->end(), afterBlock);
        m_irBuilder->SetInsertPoint(afterBlock);
    }

    void LLVMCodeGenerator::generateBreakStatement() noexcept
    {
        if (m_currentEndBlock == nullptr)
        {
            TODO("Break statement not within a loop");
        }
        m_irBuilder->CreateBr(m_currentEndBlock);
    }

    void LLVMCodeGenerator::generateSkipStatement() noexcept
    {
        if (m_currentConditionBlock == nullptr)
        {
            TODO("Skip statement not within a loop");
        }
        m_irBuilder->CreateBr(m_currentConditionBlock);
    }

    void LLVMCodeGenerator::generateReturnStatement(ReturnStatement* node) noexcept
    {
        if (node->expression().has_value())
        {
            auto* expression = node->expression().value().get();
            auto llvmReturnValue = generateExpression(expression);

            m_irBuilder->CreateRet(llvmReturnValue);
        }
        else
        {
            m_irBuilder->CreateRetVoid();
        }
    }

    llvm::Value* LLVMCodeGenerator::generateExpression(const Expression* node) noexcept
    {
        switch (node->kind())
        {
            case NodeKind::FunctionCallExpression:
            {
                return generateFunctionCallExpression(static_cast<const FunctionCallExpression*>(node));
            }
            case NodeKind::BinaryExpression:
            {
                return generateBinaryExpression(static_cast<const BinaryExpression*>(node));
            }
            case NodeKind::MemberAccessExpression:
            {
                return generateMemberAccessExpression(static_cast<const MemberAccessExpression*>(node));
            }
            case NodeKind::UnaryExpression:
            {
                return generateUnaryExpression(static_cast<const UnaryExpression*>(node));
            }
            case NodeKind::NameExpression:
            {
                return generateNameExpression(static_cast<const NameExpression*>(node));
            }
            case NodeKind::BoolLiteral:
            {
                return generateBoolLiteral(static_cast<const BoolLiteral*>(node));
            }
            case NodeKind::NumberLiteral:
            {
                return generateNumberLiteral(static_cast<const NumberLiteral*>(node));
            }
            case NodeKind::StringLiteral:
            {
                return generateStringLiteral(static_cast<const StringLiteral*>(node));
            }
            case NodeKind::GroupingExpression:
            {
                // TODO move this to a rewriter later
                const auto groupingExpression = static_cast<const GroupingExpression*>(node);
                return generateExpression(groupingExpression->expression().get());
            }
            default:
            {
                TODO("Missing NodeKind!!");
                return nullptr;
            }
        }
    }

    llvm::Value* LLVMCodeGenerator::generateUnaryExpression(const UnaryExpression* node) noexcept
    {
        switch (node->unaryOperator())
        {
            case UnaryOperatorKind::ReferenceOf:
            {
                if (node->expression()->kind() == NodeKind::NameExpression)
                {
                    const auto nameExpression = static_cast<NameExpression*>(node->expression().get());
                    const auto& name = nameExpression->name();
                    auto value = currentScope()->getVariableBinding(name);

                    if (auto localAlloca = llvm::dyn_cast<llvm::AllocaInst>(value))
                    {
                        return localAlloca;
                    }
                    else if (auto globalValue = llvm::dyn_cast<llvm::GlobalVariable>(value))
                    {
                        return globalValue;
                    }
                    else if (auto argumentValue = llvm::dyn_cast<llvm::Argument>(value))
                    {
                        return argumentValue;
                    }
                    else
                    {
                        TODO("ReferenceOf on unsupported value kind");
                        return nullptr;
                    }
                }
                else
                {
                    TODO("ReferenceOf currently only supports name expressions");
                    return nullptr;
                }
            }
            default:
            {
                TODO("Unsupported unary operator in code generation");
                return nullptr;
            }
        }
    }

    llvm::Value* LLVMCodeGenerator::generateBinaryExpression(const BinaryExpression* node) noexcept
    {
        switch (node->binaryOperator())
        {
            case BinaryOperatorKind::ConstructorCall:
            {
                const auto objectType = node->type();
                auto* llvmObjectType = GetLLVMTypeForCaraType(objectType, m_llvmModule.getContext(), m_llvmModule, m_semanticContext);
                if (llvmObjectType == nullptr)
                {
                    TODO("Constructor call target type not found");
                    return nullptr;
                }

                auto* tempObject = createLocalValue("ctor_tmp", llvmObjectType);
                auto* constructorCall = static_cast<FunctionCallExpression*>(node->rightExpression().get());
                auto constructorType = constructorCall->functionType();
                auto& constructorDefinition = m_semanticContext.getFunctionDefinition(constructorType);
                auto* llvmConstructor = getFunctionDeclaration(constructorDefinition);
                if (llvmConstructor == nullptr)
                {
                    TODO("Constructor function not found in module during constructor call generation");
                    return nullptr;
                }

                std::vector<llvm::Value*> llvmArguments;
                llvmArguments.push_back(tempObject);

                const auto& arguments = constructorCall->argumentsNode()->arguments();
                for (const auto& argument : arguments)
                {
                    auto* llvmArgumentValue = generateExpression(argument.get());
                    if (llvmArgumentValue == nullptr)
                    {
                        TODO("Argument expression produced null during constructor call generation");
                        return nullptr;
                    }

                    llvmArguments.push_back(llvmArgumentValue);
                }

                m_irBuilder->CreateCall(llvmConstructor, llvmArguments);
                return m_irBuilder->CreateLoad(llvmObjectType, tempObject, "ctor_result");
            }
            case BinaryOperatorKind::MemberAccess:
            {
                const auto type = node->type();
                if (type.kind() == TypeKind::Enum)
                {
                    const auto& enumDefinition = m_semanticContext.getEnumDefinition(type);
                    const auto baseType = enumDefinition.baseType();
                    const auto llvmType = GetLLVMTypeForCaraType(baseType, m_llvmModule.getContext());

                    const auto nameExpression = static_cast<NameExpression*>(node->rightExpression().get());
                    const auto& name = nameExpression->name();
                    const auto& enumField = enumDefinition.getFieldByName(name);

                    if(enumField.expression() != nullptr)
                    {
                        return generateExpression(enumField.expression());
                    }

                    return llvm::ConstantInt::get(llvmType, enumField.value());
                }
                else if (node->rightExpression()->kind() == NodeKind::FunctionCallExpression)
                {
                    auto* functionCallExpression = static_cast<FunctionCallExpression*>(node->rightExpression().get());
                    auto functionType = functionCallExpression->functionType();
                    auto& methodDefinition = m_semanticContext.getFunctionDefinition(functionType);
                    auto* llvmMethod = getFunctionDeclaration(methodDefinition);
                    if (llvmMethod == nullptr)
                    {
                        TODO("Method not found in module during member access call generation");
                        return nullptr;
                    }

                    std::vector<llvm::Value*> llvmArguments;
                    if (methodDefinition.functionType() == FunctionType::PublicMethod || methodDefinition.functionType() == FunctionType::PrivateMethod)
                    {
                        auto* thisPointer = getThisPointer(node->leftExpression().get());
                        if (thisPointer == nullptr)
                        {
                            TODO("This expression produced null during method call generation");
                            return nullptr;
                        }

                        llvmArguments.push_back(thisPointer);
                    }

                    const auto functionIsVariadic = llvmMethod->isVarArg();
                    const auto& arguments = functionCallExpression->argumentsNode()->arguments();
                    for (const auto& argument : arguments)
                    {
                        auto* llvmArgumentValue = generateExpression(argument.get());
                        if (llvmArgumentValue == nullptr)
                        {
                            TODO("Argument expression produced null during method call generation");
                            return nullptr;
                        }

                        llvmArgumentValue = PromoteVariadicArgumentIfNeeded(
                            llvmArgumentValue,
                            m_irBuilder.get(),
                            m_llvmModule,
                            functionIsVariadic);

                        llvmArguments.push_back(llvmArgumentValue);
                    }

                    return m_irBuilder->CreateCall(llvmMethod, llvmArguments);
                }
                else if (node->rightExpression()->kind() == NodeKind::NameExpression)
                {
                    auto* fieldPointer = generateFieldAccessPointer(node);
                    if (fieldPointer == nullptr)
                    {
                        TODO("Could not generate field access pointer");
                        return nullptr;
                    }

                    auto fieldType = node->type();
                    if (fieldType.isReference())
                    {
                        return fieldPointer;
                    }

                    auto* llvmFieldType = GetLLVMTypeForCaraType(fieldType, m_llvmModule.getContext(), m_llvmModule, m_semanticContext);
                    if (llvmFieldType == nullptr)
                    {
                        TODO("Field access type not found");
                        return nullptr;
                    }

                    return m_irBuilder->CreateLoad(llvmFieldType, fieldPointer, "field_load");
                }

                TODO("Member access operator not implemented yet");
                break;
            }
            case BinaryOperatorKind::Addition:
            {
                const auto resultType = GetLLVMTypeForCaraType(node->type(), m_llvmModule.getContext());
                auto lhs = generateExpression(node->leftExpression().get());
                auto rhs = generateExpression(node->rightExpression().get());

                lhs = dereferenceIfNeeded(node->leftExpression().get(), lhs, resultType);
                rhs = dereferenceIfNeeded(node->rightExpression().get(), rhs, resultType);

                if (resultType->isIntegerTy())
                {
                    return m_irBuilder->CreateAdd(lhs, rhs, "addtmp");
                }
                else if (resultType->isFloatingPointTy())
                {
                    return m_irBuilder->CreateFAdd(lhs, rhs, "addtmp");
                }
                else
                {
                    TODO("Unsupported type for addition operator");
                }
            }
            case BinaryOperatorKind::Subtraction:
            {
                const auto resultType = GetLLVMTypeForCaraType(node->type(), m_llvmModule.getContext());
                auto lhs = generateExpression(node->leftExpression().get());
                auto rhs = generateExpression(node->rightExpression().get());

                lhs = dereferenceIfNeeded(node->leftExpression().get(), lhs, resultType);
                rhs = dereferenceIfNeeded(node->rightExpression().get(), rhs, resultType);

                if (resultType->isIntegerTy())
                {
                    return m_irBuilder->CreateSub(lhs, rhs, "subtmp");
                }
                else if (resultType->isFloatingPointTy())
                {
                    return m_irBuilder->CreateFSub(lhs, rhs, "subtmp");
                }
                else
                {
                    TODO("Unsupported type for subtraction operator");
                }
            }
            case BinaryOperatorKind::Multiplication:
            {
                const auto resultType = GetLLVMTypeForCaraType(node->type(), m_llvmModule.getContext());
                auto lhs = generateExpression(node->leftExpression().get());
                auto rhs = generateExpression(node->rightExpression().get());

                lhs = dereferenceIfNeeded(node->leftExpression().get(), lhs, resultType);
                rhs = dereferenceIfNeeded(node->rightExpression().get(), rhs, resultType);

                if (resultType->isIntegerTy())
                {
                    return m_irBuilder->CreateMul(lhs, rhs, "multmp");
                }
                else if (resultType->isFloatingPointTy())
                {
                    return m_irBuilder->CreateFMul(lhs, rhs, "multmp");
                }
                else
                {
                    TODO("Unsupported type for multiplication operator");
                }
            }
            case BinaryOperatorKind::Division:
            {
                const auto resultType = GetLLVMTypeForCaraType(node->type(), m_llvmModule.getContext());
                auto lhs = generateExpression(node->leftExpression().get());
                auto rhs = generateExpression(node->rightExpression().get());

                lhs = dereferenceIfNeeded(node->leftExpression().get(), lhs, resultType);
                rhs = dereferenceIfNeeded(node->rightExpression().get(), rhs, resultType);

                if (resultType->isIntegerTy())
                {
                    return m_irBuilder->CreateSDiv(lhs, rhs, "divtmp");
                }
                else if (resultType->isFloatingPointTy())
                {
                    return m_irBuilder->CreateFDiv(lhs, rhs, "divtmp");
                }
                else
                {
                    TODO("Unsupported type for division operator");
                }
            }
            case BinaryOperatorKind::Equal:
            {
                auto lhs = generateExpression(node->leftExpression().get());
                auto rhs = generateExpression(node->rightExpression().get());

                auto leftType = node->leftExpression()->type().toValue();
                const auto targetType = GetLLVMTypeForCaraType(leftType, m_llvmModule.getContext());

                lhs = dereferenceIfNeeded(node->leftExpression().get(), lhs, targetType);
                rhs = dereferenceIfNeeded(node->rightExpression().get(), rhs, targetType);

                if (targetType->isIntegerTy())
                {
                    return m_irBuilder->CreateICmpEQ(lhs, rhs, "eqtmp");
                }
                else if (targetType->isFloatingPointTy())
                {
                    return m_irBuilder->CreateFCmpUEQ(lhs, rhs, "eqtmp");
                }
                else
                {
                    TODO("Unsupported type for equality operator");
                }
            }
            case BinaryOperatorKind::NotEqual:
            {
                auto lhs = generateExpression(node->leftExpression().get());
                auto rhs = generateExpression(node->rightExpression().get());

                auto leftType = node->leftExpression()->type().toValue();
                const auto targetType = GetLLVMTypeForCaraType(leftType, m_llvmModule.getContext());

                lhs = dereferenceIfNeeded(node->leftExpression().get(), lhs, targetType);
                rhs = dereferenceIfNeeded(node->rightExpression().get(), rhs, targetType);

                if (targetType->isIntegerTy())
                {
                    return m_irBuilder->CreateICmpNE(lhs, rhs, "netmp");
                }
                else if (targetType->isFloatingPointTy())
                {
                    return m_irBuilder->CreateFCmpUNE(lhs, rhs, "netmp");
                }
                else
                {
                    TODO("Unsupported type for inequality operator");
                }
            }
            case BinaryOperatorKind::LessThan:
            {
                auto lhs = generateExpression(node->leftExpression().get());
                auto rhs = generateExpression(node->rightExpression().get());

                auto leftType = node->leftExpression()->type().toValue();
                const auto targetType = GetLLVMTypeForCaraType(leftType, m_llvmModule.getContext());

                lhs = dereferenceIfNeeded(node->leftExpression().get(), lhs, targetType);
                rhs = dereferenceIfNeeded(node->rightExpression().get(), rhs, targetType);

                if (targetType->isIntegerTy())
                {
                    return m_irBuilder->CreateICmpSLT(lhs, rhs, "lttmp");
                }
                else if (targetType->isFloatingPointTy())
                {
                    return m_irBuilder->CreateFCmpULT(lhs, rhs, "lttmp");
                }
                else
                {
                    TODO("Unsupported type for less than operator");
                }
            }
            case BinaryOperatorKind::LessOrEqual:
            {
                auto lhs = generateExpression(node->leftExpression().get());
                auto rhs = generateExpression(node->rightExpression().get());

                auto leftType = node->leftExpression()->type().toValue();
                const auto targetType = GetLLVMTypeForCaraType(leftType, m_llvmModule.getContext());

                lhs = dereferenceIfNeeded(node->leftExpression().get(), lhs, targetType);
                rhs = dereferenceIfNeeded(node->rightExpression().get(), rhs, targetType);

                if (targetType->isIntegerTy())
                {
                    return m_irBuilder->CreateICmpSLE(lhs, rhs, "letmp");
                }
                else if (targetType->isFloatingPointTy())
                {
                    return m_irBuilder->CreateFCmpULE(lhs, rhs, "letmp");
                }
                else
                {
                    TODO("Unsupported type for less or equal operator");
                }
            }
            case BinaryOperatorKind::GreaterThan:
            {
                auto lhs = generateExpression(node->leftExpression().get());
                auto rhs = generateExpression(node->rightExpression().get());

                auto leftType = node->leftExpression()->type().toValue();
                const auto targetType = GetLLVMTypeForCaraType(leftType, m_llvmModule.getContext());

                lhs = dereferenceIfNeeded(node->leftExpression().get(), lhs, targetType);
                rhs = dereferenceIfNeeded(node->rightExpression().get(), rhs, targetType);

                if (targetType->isIntegerTy())
                {
                    return m_irBuilder->CreateICmpSGT(lhs, rhs, "gttmp");
                }
                else if (targetType->isFloatingPointTy())
                {
                    return m_irBuilder->CreateFCmpUGT(lhs, rhs, "gttmp");
                }
                else
                {
                    TODO("Unsupported type for greater than operator");
                }
            }
            case BinaryOperatorKind::GreaterOrEqual:
            {
                auto lhs = generateExpression(node->leftExpression().get());
                auto rhs = generateExpression(node->rightExpression().get());

                auto leftType = node->leftExpression()->type().toValue();
                const auto targetType = GetLLVMTypeForCaraType(leftType, m_llvmModule.getContext());

                lhs = dereferenceIfNeeded(node->leftExpression().get(), lhs, targetType);
                rhs = dereferenceIfNeeded(node->rightExpression().get(), rhs, targetType);

                if (targetType->isIntegerTy())
                {
                    return m_irBuilder->CreateICmpSGE(lhs, rhs, "getmp");
                }
                else if (targetType->isFloatingPointTy())
                {
                    return m_irBuilder->CreateFCmpUGE(lhs, rhs, "getmp");
                }
                else
                {
                    TODO("Unsupported type for greater or equal operator");
                }
            }
            case BinaryOperatorKind::LogicalAnd:
            {
                auto lhs = generateExpression(node->leftExpression().get());
                auto rhs = generateExpression(node->rightExpression().get());

                auto leftType = node->leftExpression()->type().toValue();
                const auto targetType = GetLLVMTypeForCaraType(leftType, m_llvmModule.getContext());

                lhs = dereferenceIfNeeded(node->leftExpression().get(), lhs, targetType);
                rhs = dereferenceIfNeeded(node->rightExpression().get(), rhs, targetType);

                if (targetType->isIntegerTy())
                {
                    return m_irBuilder->CreateAnd(lhs, rhs, "andtmp");
                }
                else
                {
                    TODO("Unsupported type for logical and operator");
                }
            }
            case BinaryOperatorKind::LogicalOr:
            {
                auto lhs = generateExpression(node->leftExpression().get());
                auto rhs = generateExpression(node->rightExpression().get());

                auto leftType = node->leftExpression()->type().toValue();
                const auto targetType = GetLLVMTypeForCaraType(leftType, m_llvmModule.getContext());

                lhs = dereferenceIfNeeded(node->leftExpression().get(), lhs, targetType);
                rhs = dereferenceIfNeeded(node->rightExpression().get(), rhs, targetType);

                if (targetType->isIntegerTy())
                {
                    return m_irBuilder->CreateOr(lhs, rhs, "ortmp");
                }
                else
                {
                    TODO("Unsupported type for logical or operator");
                }
            }
            default: 
            {
                TODO("Unsupported binary operator");
                return nullptr;
            }
        }
        return nullptr;
    }

    llvm::Value* LLVMCodeGenerator::generateNameExpression(const NameExpression* node) noexcept
    {
        const auto& name = node->name();
        auto value = currentScope()->getVariableBinding(name);

        if(auto localValue = llvm::dyn_cast<llvm::AllocaInst>(value))
        {
            return m_irBuilder->CreateLoad(localValue->getAllocatedType(), localValue, name);
        }
        else if(auto globalValue = llvm::dyn_cast<llvm::GlobalVariable>(value))
        {
            return m_irBuilder->CreateLoad(globalValue->getValueType(), globalValue, name);
        }
        else if(auto argumentValue = llvm::dyn_cast<llvm::Argument>(value))
        {
            return argumentValue;
        }

        return nullptr;
    }

    llvm::Value* LLVMCodeGenerator::generateFunctionCallExpression(const FunctionCallExpression* node) noexcept
    {
        auto functionType = node->functionType();
        auto& functionDefinition = m_semanticContext.getFunctionDefinition(functionType);
        auto functionName = functionDefinition.fullName();
        auto llvmFunction = m_llvmModule.getFunction(functionName);
        if (llvmFunction == nullptr)
        {
            TODO("Function not found in module during function call generation");
            return nullptr;
        }

        const auto functionIsVariadic = llvmFunction->isVarArg();
        std::vector<llvm::Value*> llvmArguments;
        const auto& argumentsNode = node->argumentsNode();
        const auto& arguments = argumentsNode->arguments();
        for (const auto& argument : arguments)
        {
            auto llvmArgumentValue = generateExpression(argument.get());
            if (!llvmArgumentValue)
            {
                TODO("Argument expression produced null during call generation");
                continue;
            }

            llvmArgumentValue = PromoteVariadicArgumentIfNeeded(
                llvmArgumentValue,
                m_irBuilder.get(),
                m_llvmModule,
                functionIsVariadic);

            llvmArguments.push_back(llvmArgumentValue);
        }

        return m_irBuilder->CreateCall(llvmFunction, llvmArguments);
    }

    llvm::Value* LLVMCodeGenerator::generateBoolLiteral(const BoolLiteral* node) noexcept
    {
        auto& context = m_llvmModule.getContext();
        if (node->value())
        {
            return llvm::ConstantInt::getTrue(context);
        }
        else
        {
            return llvm::ConstantInt::getFalse(context);
        }
    }

    llvm::Value* LLVMCodeGenerator::generateNumberLiteral(const NumberLiteral* node) noexcept
    {
        auto& context = m_llvmModule.getContext();
        const auto literalType = node->type();

        if (!node->hasParsedValue())
        {
            TODO("Number literal should have a parsed value after type checking");
            return nullptr;
        }

        if (literalType == Type::I32())
        {
            auto llvmType = GetLLVMTypeForCaraType(literalType, context);
            auto value = std::get<i32>(node->parsedValue().value());
            auto returnValue = llvm::ConstantInt::get(llvmType, value);
            return returnValue;
        }
        else if (literalType == Type::U8())
        {
            auto llvmType = GetLLVMTypeForCaraType(literalType, context);
            auto value = std::get<u8>(node->parsedValue().value());
            auto returnValue = llvm::ConstantInt::get(llvmType, value);
            return returnValue;
        }
        else if (literalType == Type::F32())
        {
            auto llvmType = GetLLVMTypeForCaraType(literalType, context);
            auto value = std::get<float>(node->parsedValue().value());
            auto returnValue = llvm::ConstantFP::get(context, llvm::APFloat(value));
            return returnValue;
        }

        TODO("Handle other literal types in generateNumberLiteral");
        return nullptr;
    }

    llvm::Value* LLVMCodeGenerator::generateStringLiteral(const StringLiteral* node) noexcept
    {
        auto& context = m_llvmModule.getContext();
        const auto& stringContent = node->escapedContent();
        return m_irBuilder->CreateGlobalString(stringContent, "", 0, &m_llvmModule);
    }

    llvm::Value* LLVMCodeGenerator::getPointerToField(llvm::Value* objectPtr, Type objectType, std::string_view fieldName) noexcept
    {
        auto& typeDefinition = m_semanticContext.getTypeDefinition(objectType);
        const auto& fieldDefinition = typeDefinition.tryGetFieldByName(fieldName);
        if (fieldDefinition.type() == Type::Undefined())
        {
            return nullptr;
        }

        auto* llvmObjectType = GetLLVMTypeForCaraType(objectType, m_llvmModule.getContext(), m_llvmModule, m_semanticContext);
        auto* llvmStructType = llvm::dyn_cast<llvm::StructType>(llvmObjectType);
        if (llvmStructType == nullptr)
        {
            return nullptr;
        }

        return m_irBuilder->CreateStructGEP(llvmStructType, objectPtr, static_cast<unsigned>(fieldDefinition.index()), std::string(fieldName));
    }

    llvm::FunctionType* LLVMCodeGenerator::buildFunctionType(const FunctionDefinition& functionDefinition) noexcept
    {
        auto& context = m_llvmModule.getContext();
        llvm::Type* llvmReturnType = nullptr;
        if (!functionDefinition.returnTypes().empty())
        {
            if (functionDefinition.returnTypes().size() > 1)
            {
                TODO("Handle multiple return types in function type generation");
            }
            llvmReturnType = GetLLVMTypeForCaraType(functionDefinition.returnTypes()[0], context, m_llvmModule, m_semanticContext);
        }
        else
        {
            llvmReturnType = llvm::Type::getVoidTy(context);
        }

        std::vector<llvm::Type*> llvmParameterTypes;
        const auto& parameters = functionDefinition.parameters();
        auto isVariadic = functionDefinition.isVariadic();
        auto parameterCount = (isVariadic ? parameters.size() - 1 : parameters.size());
        
        for (size_t i = 0; i < parameterCount; i++)
        {
            auto llvmParameterType = GetLLVMTypeForCaraType(parameters[i].type(), context, m_llvmModule, m_semanticContext);
            llvmParameterTypes.push_back(llvmParameterType);
        }
        
        return llvm::FunctionType::get(llvmReturnType, llvmParameterTypes, isVariadic);
    }

    bool LLVMCodeGenerator::tryGenerateConstructorCallInto(const BinaryExpression* binaryExpression, llvm::Value* destinationPtr) noexcept
    {
        auto* constructorCall = static_cast<FunctionCallExpression*>(binaryExpression->rightExpression().get());
        auto constructorType = constructorCall->functionType();
        auto& constructorDefinition = m_semanticContext.getFunctionDefinition(constructorType);
        auto* llvmConstructor = getFunctionDeclaration(constructorDefinition);
        if (llvmConstructor == nullptr)
        {
            TODO("Constructor function not found during constructor call generation");
            return false;
        }

        std::vector<llvm::Value*> llvmArguments;
        llvmArguments.push_back(destinationPtr);

        const auto& arguments = constructorCall->argumentsNode()->arguments();
        for (const auto& argument : arguments)
        {
            auto* llvmArgumentValue = generateExpression(argument.get());
            if (llvmArgumentValue == nullptr)
            {
                TODO("Argument expression produced null during constructor call generation");
                return false;
            }

            llvmArguments.push_back(llvmArgumentValue);
        }

        m_irBuilder->CreateCall(llvmConstructor, llvmArguments);
        return true;
    }

    bool LLVMCodeGenerator::tryGenerateGlobalConstructorCall(const std::string& name, BinaryExpression* binaryExpression) noexcept
    {
        auto objectType = binaryExpression->type();
        auto* llvmObjectType = GetLLVMTypeForCaraType(objectType, m_llvmModule.getContext(), m_llvmModule, m_semanticContext);
        if (llvmObjectType == nullptr)
        {
            TODO("Global constructor call target type not found");
            return false;
        }

        auto* llvmStructType = llvm::dyn_cast<llvm::StructType>(llvmObjectType);
        if (llvmStructType == nullptr)
        {
            TODO("Global constructor call target must be a struct type");
            return false;
        }

        auto* zeroInitializer = llvm::Constant::getNullValue(llvmStructType);
        auto* globalValue = createGlobalValue(name, zeroInitializer, false);
        auto* globalVariable = llvm::dyn_cast<llvm::GlobalVariable>(globalValue);
        if (globalVariable == nullptr)
        {
            TODO("Expected global variable for global constructor call");
            return false;
        }

        auto* initFunction = getOrCreateGlobalInitFunction();
        if (initFunction == nullptr)
        {
            return false;
        }

        auto* previousFunction = m_currentFunction;
        auto* previousConditionBlock = m_currentConditionBlock;
        auto* previousEndBlock = m_currentEndBlock;
        auto* previousInsertBlock = m_irBuilder->GetInsertBlock();

        m_currentFunction = initFunction;
        m_currentConditionBlock = nullptr;
        m_currentEndBlock = nullptr;

        auto& entryBlock = initFunction->getEntryBlock();
        if (entryBlock.empty())
        {
            m_irBuilder->SetInsertPoint(&entryBlock);
        }
        else if (entryBlock.getTerminator() != nullptr)
        {
            m_irBuilder->SetInsertPoint(entryBlock.getTerminator());
        }
        else
        {
            m_irBuilder->SetInsertPoint(&entryBlock);
        }

        const auto generated = tryGenerateConstructorCallInto(binaryExpression, globalVariable);

        m_currentFunction = previousFunction;
        m_currentConditionBlock = previousConditionBlock;
        m_currentEndBlock = previousEndBlock;
        if (previousInsertBlock != nullptr)
        {
            m_irBuilder->SetInsertPoint(previousInsertBlock);
        }

        return generated;
    }

    llvm::Function* LLVMCodeGenerator::getOrCreateGlobalInitFunction() noexcept
    {
        auto* initFunction = m_llvmModule.getFunction(GlobalInitFunctionName);
        if (initFunction != nullptr)
        {
            return initFunction;
        }

        auto* initFunctionType = llvm::FunctionType::get(llvm::Type::getVoidTy(m_llvmModule.getContext()), false);
        initFunction = llvm::Function::Create(initFunctionType, llvm::Function::InternalLinkage, GlobalInitFunctionName, &m_llvmModule);
        llvm::BasicBlock::Create(m_llvmModule.getContext(), "entry", initFunction);

        return initFunction;
    }

    void LLVMCodeGenerator::generateFunctionBodies() noexcept
    {
        for (const auto& functionDefinition : m_semanticContext.functionDefinitions())
        {
            m_currentType = functionDefinition.parentType();

            if (functionDefinition.functionType() == FunctionType::SynthesizedConstructor)
            {
                generateSynthesizedConstructor(functionDefinition);
                continue;
            }

            // skip builtin functions like print until we got a prelude
            if (functionDefinition.statement() == nullptr)
                continue;

            auto statement = functionDefinition.statement();
            if (statement->kind() == NodeKind::FunctionDefinitionStatement)
            {
                const auto functionStatement = static_cast<const FunctionDefinitionStatement*>(statement);
                if (!functionStatement->isExtern())
                {
                    generateFunction(functionDefinition.type(), functionStatement->bodyNode().get());
                }
            }
            else if (statement->kind() == NodeKind::MethodDefinitionStatement)
            {
                const auto methodStatement = static_cast<const MethodDefinitionStatement*>(statement);
                if (methodStatement->specialFunctionType() == SpecialFunctionType::None)
                {
                    generateFunction(functionDefinition.type(), methodStatement->bodyNode().get());
                }
            }
        }
        m_currentType = Type::Undefined();

        finishGlobalInitFunctionGeneration();
    }

    void LLVMCodeGenerator::finishGlobalInitFunctionGeneration()
    {
        if (auto* globalInitFunction = m_llvmModule.getFunction(GlobalInitFunctionName))
        {
            auto& entryBlock = globalInitFunction->getEntryBlock();
            if (!entryBlock.getTerminator())
            {
                llvm::IRBuilder<> builder(&entryBlock);
                builder.CreateRetVoid();
            }
        }
    }

    void LLVMCodeGenerator::generateBlockNode(BlockNode* body) noexcept
    {
        const auto& statements = body->statements();
        for (const auto& statement : statements)
        {
            generateNode(statement.get());
        }
    }

    void LLVMCodeGenerator::declareFunction(const FunctionDefinition& functionDefinition) noexcept
    {
        auto& functionName = functionDefinition.fullName();
        if (m_llvmModule.getFunction(functionName) != nullptr)
        {
            TODO("This shouldn't happen, " + functionName + " already declared");
        }

        auto llvmFunctionType = buildFunctionType(functionDefinition);
        auto llvmFunction = llvm::Function::Create(llvmFunctionType, llvm::Function::ExternalLinkage, functionName, &m_llvmModule);

        // mark reference parameters as nonnull
        const auto& parameters = functionDefinition.parameters();
        for (size_t i = 0; i < parameters.size(); ++i)
        {
            if (parameters[i].type().isReference())
            {
                llvmFunction->addParamAttr(i, llvm::Attribute::get(m_llvmModule.getContext(), llvm::Attribute::AttrKind::NonNull));
            }
        }
    }

    void LLVMCodeGenerator::generateTopLevelDeclarations() noexcept
    {
        generateTypeDeclarations();
        generateFunctionDeclarations();
        generateConstantDeclarations();
    }

    void LLVMCodeGenerator::generateTypeDeclarations() noexcept
    {
        auto& context = m_llvmModule.getContext();

        for (const auto& typeDefinition : m_semanticContext.typeDefinitions())
        {
            auto* llvmStructType = llvm::StructType::getTypeByName(context, typeDefinition.name());
            if (llvmStructType == nullptr)
            {
                llvmStructType = llvm::StructType::create(context, typeDefinition.name());
            }

            if (!llvmStructType->isOpaque())
                continue;

            std::vector<llvm::Type*> fieldTypes;
            const auto& fields = typeDefinition.fields();
            fieldTypes.reserve(fields.size());
            for (const auto& fieldDefinition : fields)
            {
                llvm::Type* llvmFieldType = GetLLVMTypeForCaraType(
                    fieldDefinition.type(),
                    context,
                    m_llvmModule,
                    m_semanticContext);

                if (llvmFieldType == nullptr)
                {
                    TODO("Unsupported field type in type declaration generation");
                    continue;
                }

                fieldTypes.push_back(llvmFieldType);
            }

            llvmStructType->setBody(fieldTypes, false);
        }
    }

    void LLVMCodeGenerator::generateConstantDeclarations() noexcept
    {
        for (const auto& constantDefinition : m_semanticContext.constantDefinitions())
        {
            const auto expression = constantDefinition.expression();
            if (expression == nullptr)
                continue;

            if (expression->kind() == NodeKind::BinaryExpression)
            {
                auto* binaryExpression = const_cast<BinaryExpression*>(static_cast<const BinaryExpression*>(expression));
                if (binaryExpression->binaryOperator() == BinaryOperatorKind::ConstructorCall)
                {
                    if (tryGenerateGlobalConstructorCall(constantDefinition.name(), binaryExpression))
                    {
                        continue;
                    }
                }
            }

            auto llvmValue = generateExpression(expression);
            if (!llvmValue)
            {
                TODO("Constant expression produced null during codegen");
                continue;
            }

            if (auto llvmConstant = llvm::dyn_cast<llvm::Constant>(llvmValue))
            {
                createGlobalValue(constantDefinition.name(), llvmConstant, true);
            }
            else
            {
                TODO("Global constant must be an llvm::Constant");
            }
        }
    }

    void LLVMCodeGenerator::generateFunctionDeclarations() noexcept
    {
        for (const auto& functionDefinition : m_semanticContext.functionDefinitions())
        {
            declareFunction(functionDefinition);
        }
    }

    llvm::GlobalValue* LLVMCodeGenerator::createGlobalValue(
        const std::string& name, 
        llvm::Constant* constant, 
        bool isConst) noexcept
    {
        auto value = m_llvmModule.getOrInsertGlobal(name, constant->getType());
        value->setAlignment(llvm::MaybeAlign(4));
        value->setConstant(isConst);
        value->setInitializer(constant);
        currentScope()->addVariableBinding(name, value);

        return value;
    }

    llvm::Value* LLVMCodeGenerator::createLocalValue(const std::string& name, llvm::Type* type) noexcept
    {
        auto& entryBlock = m_currentFunction->getEntryBlock();
        auto insertPoint = entryBlock.getFirstInsertionPt();
        while (insertPoint != entryBlock.end() && llvm::isa<llvm::AllocaInst>(&*insertPoint))
        {
            ++insertPoint;
        }

        llvm::IRBuilder<> builder(&entryBlock, insertPoint);

        auto value = builder.CreateAlloca(type, 0, name);
        currentScope()->addVariableBinding(name, value);

        return value;
    }

    void LLVMCodeGenerator::pushScope()
    {
        auto parent = m_scopes.back().get();
        m_scopes.emplace_back(std::make_unique<LLVMScope>(parent));
    }

    void LLVMCodeGenerator::popScope()
    {
        m_scopes.pop_back();
        if (m_scopes.size() == 0)
        {
            TODO("Popped too many scopes");
        }
    }

    LLVMScope* LLVMCodeGenerator::currentScope() const noexcept
    {
        return m_scopes.back().get();
    }

    bool generateLLVMModule(SemanticContext& semanticContext, llvm::Module& llvmModule) noexcept
    {
        LLVMCodeGenerator generator{ semanticContext, llvmModule };
        return generator.generate();
    }

    llvm::Value* LLVMCodeGenerator::dereferenceIfNeeded(Expression* expression, llvm::Value* value, llvm::Type* targetType) noexcept
    {
        if (expression != nullptr && !expression->type().isReference())
            return value;

        // we need to deref the pointer for the values, and we need to handle 
        // different pointer sources (load, global variable, argument, alloca)

        if (auto loadInstruction = llvm::dyn_cast<llvm::LoadInst>(value))
        {
            if (loadInstruction->getType()->isPointerTy())
            {
                return m_irBuilder->CreateLoad(targetType, loadInstruction, "ref_load");
            }
            return loadInstruction;
        }

        if (auto globalVariable = llvm::dyn_cast<llvm::GlobalVariable>(value))
        {
            if (globalVariable->getValueType()->isPointerTy())
            {
                return m_irBuilder->CreateLoad(targetType, globalVariable, "ref_load");
            }
            return m_irBuilder->CreateLoad(globalVariable->getValueType(), globalVariable, "load");
        }

        if (auto argument = llvm::dyn_cast<llvm::Argument>(value))
        {
            if (argument->getType()->isPointerTy())
            {
                return m_irBuilder->CreateLoad(targetType, argument, "ref_load");
            }
            return argument;
        }

        if (auto allocaInstruction = llvm::dyn_cast<llvm::AllocaInst>(value))
        {
            if (allocaInstruction->getAllocatedType()->isPointerTy())
            {
                auto ptr = m_irBuilder->CreateLoad(allocaInstruction->getAllocatedType(), allocaInstruction, "ref_load_tmp");
                return m_irBuilder->CreateLoad(targetType, ptr, "ref_load");
            }
            return m_irBuilder->CreateLoad(allocaInstruction->getAllocatedType(), allocaInstruction, "load");
        }

        if (value->getType()->isPointerTy())
        {
            return m_irBuilder->CreateLoad(targetType, value, "ref_load");
        }

        return value;
    }

    llvm::Value* LLVMCodeGenerator::getPointerForAssignment(Expression* expr, llvm::Value* value) noexcept
    {
        if (!value)
        {
            return nullptr;
        }

        if (auto loadInstruction = llvm::dyn_cast<llvm::LoadInst>(value))
        {
            return loadInstruction;
        }
        if (auto globalVariable = llvm::dyn_cast<llvm::GlobalVariable>(value))
        {
            if (globalVariable->getValueType()->isPointerTy())
            {
                return m_irBuilder->CreateLoad(globalVariable->getValueType(), globalVariable, "ref_load_lhs");
            }
            return globalVariable;
        }
        if (auto argument = llvm::dyn_cast<llvm::Argument>(value))
        {
            return argument;
        }
        if (auto allocaInstruction = llvm::dyn_cast<llvm::AllocaInst>(value))
        {
            if (allocaInstruction->getAllocatedType()->isPointerTy())
            {
                return m_irBuilder->CreateLoad(allocaInstruction->getAllocatedType(), allocaInstruction, "ref_load_lhs");
            }
            return allocaInstruction;
        }

        if (value->getType()->isPointerTy())
        {
            return value;
        }

        return nullptr;
    }

    llvm::Value* LLVMCodeGenerator::getThisPointer(Expression* thisExpression) noexcept
    {
        if (thisExpression == nullptr)
        {
            return nullptr;
        }

        if (thisExpression->type().isReference())
        {
            auto* thisValue = generateExpression(thisExpression);
            if (thisValue == nullptr)
            {
                return nullptr;
            }

            return getPointerForAssignment(thisExpression, thisValue);
        }

        if (thisExpression->kind() == NodeKind::NameExpression)
        {
            auto* nameExpression = static_cast<NameExpression*>(thisExpression);
            auto* binding = currentScope()->getVariableBinding(nameExpression->name());
            if (binding != nullptr)
            {
                if (auto* allocaInstruction = llvm::dyn_cast<llvm::AllocaInst>(binding))
                {
                    return allocaInstruction;
                }

                if (auto* globalVariable = llvm::dyn_cast<llvm::GlobalVariable>(binding))
                {
                    return globalVariable;
                }

                if (auto* argument = llvm::dyn_cast<llvm::Argument>(binding))
                {
                    if (argument->getType()->isPointerTy())
                    {
                        return argument;
                    }

                    const auto temporaryThisName = std::string(nameExpression->name()) + ".addr";
                    if (m_currentFunction != nullptr)
                    {
                        auto& entryBlock = m_currentFunction->getEntryBlock();
                        for (auto& instruction : entryBlock)
                        {
                            auto* existingAlloca = llvm::dyn_cast<llvm::AllocaInst>(&instruction);
                            if (existingAlloca == nullptr)
                            {
                                continue;
                            }

                            if (existingAlloca->getName() == temporaryThisName)
                            {
                                return existingAlloca;
                            }
                        }
                    }

                    auto thisType = thisExpression->type().toValue();
                    auto* llvmThisType = GetLLVMTypeForCaraType(thisType, m_llvmModule.getContext(), m_llvmModule, m_semanticContext);
                    if (llvmThisType == nullptr)
                    {
                        return nullptr;
                    }

                    auto* temporaryThis = createLocalValue(temporaryThisName, llvmThisType);
                    m_irBuilder->CreateStore(argument, temporaryThis);
                    return temporaryThis;
                }
            }
        }

        auto* thisValue = generateExpression(thisExpression);
        if (thisValue == nullptr)
        {
            return nullptr;
        }

        if (auto* thisLoad = llvm::dyn_cast<llvm::LoadInst>(thisValue))
        {
            return thisLoad->getPointerOperand();
        }

        auto thisType = thisExpression->type().toValue();
        auto* llvmThisType = GetLLVMTypeForCaraType(thisType, m_llvmModule.getContext(), m_llvmModule, m_semanticContext);
        if (llvmThisType == nullptr)
        {
            return nullptr;
        }

        auto* temporaryThis = createLocalValue("method_this_tmp", llvmThisType);
        m_irBuilder->CreateStore(thisValue, temporaryThis);
        return temporaryThis;
    }

    llvm::Value* LLVMCodeGenerator::generateFieldAccessPointer(const BinaryExpression* node) noexcept
    {
        if (node->rightExpression()->kind() != NodeKind::NameExpression)
        {
            return nullptr;
        }

        auto* objectPointer = getThisPointer(node->leftExpression().get());
        if (objectPointer == nullptr)
        {
            return nullptr;
        }

        auto objectType = node->leftExpression()->type().toValue();
        auto* fieldNameExpression = static_cast<NameExpression*>(node->rightExpression().get());
        return getPointerToField(objectPointer, objectType, fieldNameExpression->name());
    }

    llvm::Value* LLVMCodeGenerator::generateMemberAccessExpression(const MemberAccessExpression* node) noexcept
    {
        if (m_currentFunction == nullptr)
        {
            TODO("Member access expression requires current function");
            return nullptr;
        }

        auto* innerExpression = node->expression().get();
        if (innerExpression->kind() == NodeKind::NameExpression)
        {
            auto* thisValue = currentScope()->getVariableBinding("this");
            if (thisValue == nullptr)
            {
                TODO("Method member access requires this binding");
                return nullptr;
            }

            auto* thisPointer = getPointerForAssignment(nullptr, thisValue);
            if (thisPointer == nullptr)
            {
                TODO("Could not get this pointer for member access expression");
                return nullptr;
            }

            auto* fieldNameExpression = static_cast<NameExpression*>(innerExpression);
            auto* fieldPointer = getPointerToField(thisPointer, m_currentType, fieldNameExpression->name());
            if (fieldPointer == nullptr)
            {
                TODO("Could not get field pointer for member access expression");
                return nullptr;
            }

            auto fieldType = node->type();
            if (fieldType.isReference())
            {
                return fieldPointer;
            }

            auto* llvmFieldType = GetLLVMTypeForCaraType(fieldType, m_llvmModule.getContext(), m_llvmModule, m_semanticContext);
            if (llvmFieldType == nullptr)
            {
                TODO("Member access field type not found");
                return nullptr;
            }

            return m_irBuilder->CreateLoad(llvmFieldType, fieldPointer, "member_field_load");
        }
        else if (innerExpression->kind() == NodeKind::FunctionCallExpression)
        {
            auto* thisValue = currentScope()->getVariableBinding("this");
            if (thisValue == nullptr)
            {
                TODO("Method member call requires this binding");
                return nullptr;
            }

            auto* thisPointer = getPointerForAssignment(nullptr, thisValue);
            if (thisPointer == nullptr)
            {
                TODO("Could not get this pointer for member call expression");
                return nullptr;
            }

            auto* functionCallExpression = static_cast<FunctionCallExpression*>(innerExpression);
            auto functionType = functionCallExpression->functionType();
            auto& methodDefinition = m_semanticContext.getFunctionDefinition(functionType);
            auto* llvmMethod = getFunctionDeclaration(methodDefinition);
            if (llvmMethod == nullptr)
            {
                TODO("Method not found in module during member access call generation");
                return nullptr;
            }

            std::vector<llvm::Value*> llvmArguments;
            llvmArguments.push_back(thisPointer);

            const auto functionIsVariadic = llvmMethod->isVarArg();
            const auto& arguments = functionCallExpression->argumentsNode()->arguments();
            for (const auto& argument : arguments)
            {
                auto* llvmArgumentValue = generateExpression(argument.get());
                if (llvmArgumentValue == nullptr)
                {
                    TODO("Argument expression produced null during member call generation");
                    return nullptr;
                }

                llvmArgumentValue = PromoteVariadicArgumentIfNeeded(
                    llvmArgumentValue,
                    m_irBuilder.get(),
                    m_llvmModule,
                    functionIsVariadic);

                llvmArguments.push_back(llvmArgumentValue);
            }

            return m_irBuilder->CreateCall(llvmMethod, llvmArguments);
        }

        TODO("Unsupported member access expression kind in code generation");
        return nullptr;
    }
}
