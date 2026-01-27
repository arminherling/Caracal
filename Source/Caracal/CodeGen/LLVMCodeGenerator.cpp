#include "LLVMCodeGenerator.h"
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/ADT/APFloat.h>

namespace Caracal
{
    [[nodiscard]] static auto InitializeTypeToLLVMType(llvm::LLVMContext& context) noexcept
    {
        return std::unordered_map<Type, llvm::Type*>{
            { Type::Bool(), llvm::Type::getInt1Ty(context) },
            { Type::I32(), llvm::Type::getInt32Ty(context) },
            { Type::F32(), llvm::Type::getFloatTy(context) },
            //{ Type::String(), std::string_view("std::string") },
        };
    }

    [[nodiscard]] static llvm::Type* GetLLVMTypeForCaraType(Type type, llvm::LLVMContext& context) noexcept
    {
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

    LLVMCodeGenerator::LLVMCodeGenerator(
        const ParseTree& parseTree, 
        TypeDatabase& typeDatabase,
        llvm::Module& module)
        : m_parseTree{ parseTree }
        , m_typeDatabase{ typeDatabase }
        , m_module{ module }
        , m_currentScope{ Scope::Global }
        , m_currentFunction{ nullptr }
        , m_currentBasicBlock{ nullptr }
    {
        m_scopes.emplace_back(std::make_unique<LLVMScope>(nullptr));
    }

    bool LLVMCodeGenerator::generate()
    {
        setupPrintfFunctionDeclaration();

        for (const auto& statement : m_parseTree.statements())
        {
            generateNode(statement.get());
        }

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
            case NodeKind::FunctionDefinitionStatement:
            {
                generateFunctionDefinition((FunctionDefinitionStatement*)node);
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
        
        if (m_currentScope == Scope::Global)
        {
            const auto llvmConstant = llvm::dyn_cast<llvm::Constant>(llvmValue);
            createGlobalValue(name, llvmConstant, true);
        }
        else
        {
            const auto llvmType = llvmValue->getType();
            const auto localValue = createLocalValue(name, llvmType);
            
            llvm::IRBuilder<> builder(m_currentBasicBlock);
            builder.CreateStore(llvmValue, localValue);
        }
    }

    void LLVMCodeGenerator::generateFunctionDefinition(FunctionDefinitionStatement* node) noexcept
    {
        const auto oldScope = m_currentScope;
        m_currentScope = Scope::Function;
        pushScope();

        auto functionType = node->type();
        auto functionDefinition = m_typeDatabase.getFunctionDefinition(functionType);
        auto functionName = functionDefinition.name();

        m_currentFunction = m_module.getFunction(functionName);
        if (m_currentFunction == nullptr)
        {
            auto llvmFunctionType = generateFunctionType(functionDefinition);
            // TODO handle linkage types
            m_currentFunction = llvm::Function::Create(llvmFunctionType, llvm::Function::ExternalLinkage, functionName, &m_module);
        }

        const auto& body = node->bodyNode();
        generateFunctionBody(body.get(), m_currentFunction);

        popScope();
        m_currentFunction = nullptr;
        m_currentScope = oldScope; // Reset the scope after generating the function definition
    }

    void LLVMCodeGenerator::generateExpressionStatement(ExpressionStatement* node) noexcept
    {
        if (m_currentBasicBlock == nullptr)
        {
            TODO("Expression statement outside of a basic block");
        }
        llvm::IRBuilder<> builder(m_currentBasicBlock);
        const auto expression = node->expression().get();
        auto llvmValue = generateExpression(expression);
    }

    void LLVMCodeGenerator::generateReturnStatement(ReturnStatement* node) noexcept
    {
        if (m_currentBasicBlock == nullptr)
        {
            TODO("Return statement outside of a basic block");
        }

        llvm::IRBuilder<> builder(m_currentBasicBlock);
        if (node->expression().has_value())
        {
            auto llvmReturnValue = generateExpression(node->expression().value().get());
            builder.CreateRet(llvmReturnValue);
        }
        else
        {
            builder.CreateRetVoid();
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
            default:
            {
                TODO("Missing NodeKind!!");
                break;
            }
        }
    }

    llvm::Value* LLVMCodeGenerator::generateFunctionCallExpression(FunctionCallExpression* node) noexcept
    {
        if (m_currentBasicBlock == nullptr)
        {
            TODO("Function call expression outside of a basic block");
        }

        llvm::IRBuilder<> builder(m_currentBasicBlock);
        const auto nameExpression = node->nameExpression().get();
        const auto functionName = m_parseTree.tokens().getLexeme(nameExpression->nameToken());
        auto argumentsNode = node->argumentsNode().get();

        const auto maybeMappedFunctionName = MapFunctionNameToExternFunction(functionName);
        auto llvmFunction = m_module.getFunction(maybeMappedFunctionName);
        if (llvmFunction == nullptr)
        {
            TODO("Function not found in module during function call generation");
        }

        const auto functionIsVariadic = llvmFunction->isVarArg();
        std::vector<llvm::Value*> llvmArguments;
        const auto& arguments = argumentsNode->arguments();
        for (const auto& argument : arguments)
        {
            auto llvmArgumentValue = generateExpression(argument.get());
            // we need to promote float to double for variadic functions like printf
            if (functionIsVariadic && llvmArgumentValue->getType()->isFloatTy())
            {
                llvmArgumentValue = builder.CreateFPExt(llvmArgumentValue, llvm::Type::getDoubleTy(m_module.getContext()));
            }
            llvmArguments.push_back(llvmArgumentValue);
        }
        
        return builder.CreateCall(llvmFunction, llvmArguments);
    }

    llvm::Value* LLVMCodeGenerator::generateNameExpression(NameExpression* node) noexcept
    {
        const auto& name = node->name();
        auto value = currentScope()->getVariableBinding(name);

        if(auto localValue = llvm::dyn_cast<llvm::AllocaInst>(value))
        {
            llvm::IRBuilder<> builder(m_currentBasicBlock);
            return builder.CreateLoad(localValue->getAllocatedType(), localValue, name);
        }
        else if(auto globalValue = llvm::dyn_cast<llvm::GlobalVariable>(value))
        {
            llvm::IRBuilder<> builder(m_currentBasicBlock);
            return builder.CreateLoad(globalValue->getValueType(), globalValue, name);
        }

        return nullptr;
    }

    llvm::Value* LLVMCodeGenerator::generateBoolLiteral(BoolLiteral* node) noexcept
    {
        auto& context = m_module.getContext();
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
        auto& context = m_module.getContext();
        const auto literalType = node->type();
        const auto lexeme = m_parseTree.tokens().getLexeme(node->literalToken());

        if (literalType == Type::I32())
        {
            auto llvmType = GetLLVMTypeForCaraType(literalType, context);
            auto value = std::stoi(lexeme.data());
            auto returnValue = llvm::ConstantInt::get(llvmType, value);
            return returnValue;
        }
        else if (literalType == Type::F32())
        {
            auto llvmType = GetLLVMTypeForCaraType(literalType, context);
            auto value = std::stof(lexeme.data());
            auto returnValue = llvm::ConstantFP::get(context, llvm::APFloat(value));
            return returnValue;
        }
         
        TODO("Handle other literal types in generateNumberLiteral");
        return nullptr;
    }

    llvm::Value* LLVMCodeGenerator::generateStringLiteral(StringLiteral* node) noexcept
    {
        auto& context = m_module.getContext();
        llvm::IRBuilder<> builder(context);
        const auto& stringContent = node->escapedContent();
        return builder.CreateGlobalString(stringContent, "", 0, &m_module);
    }

    llvm::FunctionType* LLVMCodeGenerator::generateFunctionType(FunctionDefinition& functionDefinition) noexcept
    {
        auto& context = m_module.getContext();
        const auto& returnTypes = functionDefinition.returnTypes();
        const auto hasReturnTypes = !returnTypes.empty();

        llvm::Type* llvmReturnType = nullptr;
        if (!hasReturnTypes)
        {
            llvmReturnType = llvm::Type::getVoidTy(context);
        }
        else
        {
            if(returnTypes.size() > 1)
            {
                TODO("Handle multiple return types in function type generation");
            }
            llvmReturnType = GetLLVMTypeForCaraType(returnTypes[0], context);
        }

        std::vector<llvm::Type*> llvmParameterTypes;
        const auto& parameters = functionDefinition.parameters();
        for(const auto& parameterType : parameters)
        {
            auto llvmParameterType = GetLLVMTypeForCaraType(parameterType, context);
            llvmParameterTypes.push_back(llvmParameterType);
        }

        auto llvmFunctionType = llvm::FunctionType::get(llvmReturnType, llvmParameterTypes, false);
        return llvmFunctionType;
    }

    void LLVMCodeGenerator::generateFunctionBody(BlockNode* body, llvm::Function* llvmFunction) noexcept
    {
        auto& context = m_module.getContext();
        m_currentBasicBlock = llvm::BasicBlock::Create(context, "entry", llvmFunction);

        const auto& statements = body->statements();
        for (const auto& statement : statements)
        {
            generateNode(statement.get());
        }

        m_currentBasicBlock = nullptr;
    }

    void LLVMCodeGenerator::setupPrintfFunctionDeclaration() noexcept
    {
        auto& context = m_module.getContext();
        auto bytePtrType = llvm::Type::getInt8Ty(context)->getPointerTo();

        m_module.getOrInsertFunction("printf",
            llvm::FunctionType::get(
                llvm::Type::getInt32Ty(context),
                bytePtrType,
                true
            )
        );
    }

    llvm::GlobalValue* LLVMCodeGenerator::createGlobalValue(
        const std::string& name, 
        llvm::Constant* constant, 
        bool isConst) noexcept
    {
        auto value = m_module.getOrInsertGlobal(name, constant->getType());
        value->setAlignment(llvm::MaybeAlign(4));
        value->setConstant(isConst);
        value->setInitializer(constant);
        currentScope()->addVariableBinding(name, value);

        return value;
    }

    llvm::Value* LLVMCodeGenerator::createLocalValue(const std::string& name, llvm::Type* type) noexcept
    {
        llvm::IRBuilder<> builder(&m_currentFunction->getEntryBlock());

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

    bool generateLLVMModule(const ParseTree& parseTree, TypeDatabase& typeDatabase, llvm::Module& module) noexcept
    {
        LLVMCodeGenerator generator{ parseTree, typeDatabase, module };
        return generator.generate();
    }
}
