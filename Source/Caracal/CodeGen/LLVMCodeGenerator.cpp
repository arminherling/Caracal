#include "LLVMCodeGenerator.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/ADT/APFloat.h>
#include <Caracal/Syntax/GroupingExpression.h>

namespace Caracal
{
    [[nodiscard]] static auto InitializeTypeToLLVMType(llvm::LLVMContext& context) noexcept
    {
        return std::unordered_map<Type, llvm::Type*>{
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

        static const auto llvmTypes = InitializeTypeToLLVMType(context);
        if (const auto result = llvmTypes.find(type); result != llvmTypes.end())
            return result->second;

        return nullptr;
    }

    [[nodiscard]] static auto InitializeBuiltinFunctions() noexcept
    {
        return std::unordered_map<std::string_view, std::string_view>{
            { std::string_view("print"), std::string_view("printf") },
        };
    }

    [[nodiscard]] static std::string_view MapFunctionNameToExternFunction(std::string_view functionName) noexcept
    {
        static const auto builtinFunctions = InitializeBuiltinFunctions();
        if (const auto result = builtinFunctions.find(functionName); result != builtinFunctions.end())
            return result->second;

        return functionName;
    }

    [[nodiscard]] static void SetupFunctionParameters(
        FunctionDefinition& functionDefinition,
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
                llvm::IRBuilder<> entryBuilder(&llvmFunction->getEntryBlock(), llvmFunction->getEntryBlock().getFirstInsertionPt());
                auto allocated = entryBuilder.CreateAlloca(argument.getType(), 0, parameter.name());
                scope->addVariableBinding(parameter.name(), allocated);
                irBuilderBase->CreateStore(&argument, allocated);
            }
        }
    }

    LLVMCodeGenerator::LLVMCodeGenerator(
        Module& caracalModule,
        llvm::Module& llvmModule)
        : m_caracalModule{ caracalModule }
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
        declareAllFunctions();
        generateFunctionBodies();

        return true;
    }

    void LLVMCodeGenerator::generateNode(Node* node) noexcept
    {
        switch (node->kind())
        {
            case NodeKind::ConstantDeclaration:
            {
                generateConstantDeclaration((ConstantDeclaration*)node);
                break;
            }
            case NodeKind::VariableDeclaration:
            {
                generateVariableDeclaration((VariableDeclaration*)node);
                break;
            }
            case NodeKind::AssignmentStatement:
            {
                generateAssignmentStatement((AssignmentStatement*)node);
                break;
            }
            case NodeKind::FunctionDefinitionStatement:
            {
                auto functionDefinitionNode = (FunctionDefinitionStatement*)node;
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
                generateTypeDefinition((TypeDefinitionStatement*)node);
                break;
            }
            case NodeKind::IfStatement:
            {
                generateIfStatement((IfStatement*)node);
                break;
            }
            case NodeKind::WhileStatement:
            {
                generateWhileStatement((WhileStatement*)node);
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
                generateExpressionStatement((ExpressionStatement*)node);
                break;
            }
            case NodeKind::ReturnStatement:
            {
                generateReturnStatement((ReturnStatement*)node);
                break;
            }
            case NodeKind::BlockNode:
            {
                generateBlockNode((BlockNode*)node);
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
        const auto nameExpression = (NameExpression*)leftExpression;
        const auto& name = nameExpression->name();
        
        auto llvmValue = generateExpression(node->rightExpression().get());
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
        const auto nameExpression = (NameExpression*)leftExpression;
        const auto& name = nameExpression->name();

        auto llvmValue = generateExpression(node->rightExpression().get());
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
        auto& functionDefinition = m_caracalModule.getFunctionDefinition(functionType);
        auto& functionName = functionDefinition.fullName();

        m_currentFunction = getFunctionDeclaration(functionDefinition);

        pushScope();

        auto entry = llvm::BasicBlock::Create(m_llvmModule.getContext(), "entry", m_currentFunction);
        m_irBuilder->SetInsertPoint(entry);

        SetupFunctionParameters(functionDefinition, m_currentFunction, currentScope(), m_irBuilder.get());

        generateBlockNode(body);

        auto llvmReturnType = m_currentFunction->getReturnType();
        if (llvmReturnType->isVoidTy())
        {
            // we need to add a return void
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

        // try mapped name (print -> printf)
        const auto maybeMappedFunctionName = MapFunctionNameToExternFunction(functionName);
        if (maybeMappedFunctionName != functionName)
        {
            llvmFunction = m_llvmModule.getFunction(maybeMappedFunctionName);
            if (llvmFunction != nullptr)
                return llvmFunction;
        }

        TODO("This shouldn't happen");
        return nullptr;
    }

    void LLVMCodeGenerator::generateTypeDefinition(TypeDefinitionStatement* node) noexcept
    {
        auto thisType = node->type();
        auto typeDefinition = m_caracalModule.getTypeDefinition(thisType);

        const auto& statements = node->bodyNode()->statements();
        for(const auto& statement : statements)
        {
            if(statement->kind() == NodeKind::MethodDefinitionStatement)
            {
                auto method = (MethodDefinitionStatement*)statement.get();
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
            auto llvmReturnValue = generateExpression(node->expression().value().get());
            m_irBuilder->CreateRet(llvmReturnValue);
        }
        else
        {
            m_irBuilder->CreateRetVoid();
        }
    }

    llvm::Value* LLVMCodeGenerator::generateExpression(Expression* node) noexcept
    {
        switch (node->kind())
        {
            case NodeKind::FunctionCallExpression:
            {
                return generateFunctionCallExpression((FunctionCallExpression*)node);
            }
            case NodeKind::BinaryExpression:
            {
                return generateBinaryExpression((BinaryExpression*)node);
            }
            case NodeKind::UnaryExpression:
            {
                return generateUnaryExpression((UnaryExpression*)node);
            }
            case NodeKind::NameExpression:
            {
                return generateNameExpression((NameExpression*)node);
            }
            case NodeKind::BoolLiteral:
            {
                return generateBoolLiteral((BoolLiteral*)node);
            }
            case NodeKind::NumberLiteral:
            {
                return generateNumberLiteral((NumberLiteral*)node);
            }
            case NodeKind::StringLiteral:
            {
                return generateStringLiteral((StringLiteral*)node);
            }
            case NodeKind::GroupingExpression:
            {
                // TODO move this to a rewriter later
                const auto groupingExpression = (GroupingExpression*)node;
                return generateExpression(groupingExpression->expression().get());
            }
            default:
            {
                TODO("Missing NodeKind!!");
                return nullptr;
            }
        }
    }

    llvm::Value* LLVMCodeGenerator::generateUnaryExpression(UnaryExpression* node) noexcept
    {
        switch (node->unaryOperator())
        {
            case UnaryOperatorKind::ReferenceOf:
            {
                if (node->expression()->kind() == NodeKind::NameExpression)
                {
                    const auto nameExpression = (NameExpression*)node->expression().get();
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

    llvm::Value* LLVMCodeGenerator::generateBinaryExpression(BinaryExpression* node) noexcept
    {
        switch (node->binaryOperator())
        {
            case BinaryOperatorKind::MemberAccess:
            {
                const auto type = node->type();
                if (type.kind() == TypeKind::Enum)
                {
                    const auto& enumDefinition = m_caracalModule.getEnumDefinition(type);
                    const auto baseType = enumDefinition.baseType();
                    const auto llvmType = GetLLVMTypeForCaraType(baseType, m_llvmModule.getContext());

                    const auto nameExpression = (NameExpression*)node->rightExpression().get();
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
                    return generateExpression(node->rightExpression().get());
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

    llvm::Value* LLVMCodeGenerator::generateNameExpression(NameExpression* node) noexcept
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

    llvm::Value* LLVMCodeGenerator::generateFunctionCallExpression(FunctionCallExpression* node) noexcept
    {
        auto functionType = node->functionType();
        auto& functionDefinition = m_caracalModule.getFunctionDefinition(functionType);
        auto functionName = functionDefinition.fullName();
        const auto maybeMappedFunctionName = MapFunctionNameToExternFunction(functionName);
        auto llvmFunction = m_llvmModule.getFunction(maybeMappedFunctionName);
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
            if (functionIsVariadic)
            {
                // we need to promote bool to int32 for variadic functions like printf
                if (llvmArgumentValue->getType()->isIntegerTy(1) || llvmArgumentValue->getType()->isIntegerTy(8))
                {
                    llvmArgumentValue = m_irBuilder->CreateZExt(llvmArgumentValue, llvm::Type::getInt32Ty(m_llvmModule.getContext()));
                }
                // we need to promote float to double for variadic functions like printf
                else if (llvmArgumentValue->getType()->isFloatTy())
                {
                    llvmArgumentValue = m_irBuilder->CreateFPExt(llvmArgumentValue, llvm::Type::getDoubleTy(m_llvmModule.getContext()));
                }
            }
            llvmArguments.push_back(llvmArgumentValue);
        }

        return m_irBuilder->CreateCall(llvmFunction, llvmArguments);
    }

    llvm::Value* LLVMCodeGenerator::generateBoolLiteral(BoolLiteral* node) noexcept
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

    llvm::Value* LLVMCodeGenerator::generateNumberLiteral(NumberLiteral* node) noexcept
    {
        auto& context = m_llvmModule.getContext();
        const auto literalType = node->type();
        const auto& lexeme = node->literalLexeme();

        if (literalType == Type::I32())
        {
            auto llvmType = GetLLVMTypeForCaraType(literalType, context);
            auto value = std::stoi(lexeme);
            auto returnValue = llvm::ConstantInt::get(llvmType, value);
            return returnValue;
        }
        else if (literalType == Type::F32())
        {
            auto llvmType = GetLLVMTypeForCaraType(literalType, context);
            auto value = std::stof(lexeme);
            auto returnValue = llvm::ConstantFP::get(context, llvm::APFloat(value));
            return returnValue;
        }

        TODO("Handle other literal types in generateNumberLiteral");
        return nullptr;
    }

    llvm::Value* LLVMCodeGenerator::generateStringLiteral(StringLiteral* node) noexcept
    {
        auto& context = m_llvmModule.getContext();
        const auto& stringContent = node->escapedContent();
        return m_irBuilder->CreateGlobalString(stringContent, "", 0, &m_llvmModule);
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
            llvmReturnType = GetLLVMTypeForCaraType(functionDefinition.returnTypes()[0], context);
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
            auto llvmParameterType = GetLLVMTypeForCaraType(parameters[i].type(), context);
            llvmParameterTypes.push_back(llvmParameterType);
        }

        return llvm::FunctionType::get(llvmReturnType, llvmParameterTypes, isVariadic);
    }

    void LLVMCodeGenerator::generateFunctionBodies() noexcept
    {
        for (const auto& definitionPair : m_caracalModule.functionDefinitions())
        {
            const auto& functionDefinition = definitionPair.second;
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

    void LLVMCodeGenerator::declareAllFunctions() noexcept
    {
        for (const auto& definitionPair : m_caracalModule.functionDefinitions())
        {
            const auto& functionDefinition = definitionPair.second;
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
        llvm::IRBuilder<> builder(&m_currentFunction->getEntryBlock(), 
                              m_currentFunction->getEntryBlock().getFirstInsertionPt());

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

    bool generateLLVMModule(Module& caracalModule, llvm::Module& llvmModule) noexcept
    {
        LLVMCodeGenerator generator{ caracalModule, llvmModule };
        return generator.generate();
    }

    llvm::Value* LLVMCodeGenerator::dereferenceIfNeeded(Expression* expression, llvm::Value* value, llvm::Type* targetType) noexcept
    {
        if (!expression->type().isReference())
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
}
