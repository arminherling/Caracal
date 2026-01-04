#include "LLVMCodeGenerator.h"
#include "LLVMCodeGenerator.h"
#include "LLVMCodeGenerator.h"

#include <llvm/IR/Verifier.h>
#include <llvm/IR/IRBuilder.h>

namespace Caracal
{
    [[nodiscard]] static auto InitializeTypeToLLVMType(llvm::LLVMContext& context) noexcept
    {
        return std::unordered_map<Type, llvm::Type*>{
            //{ Type::Bool(), std::string_view("bool") },
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

    LLVMCodeGenerator::LLVMCodeGenerator(const ParseTree& parseTree, llvm::Module& module)
        : m_parseTree{ parseTree }
        , m_module{ module }
        , m_currentScope{ Scope::Global }
        , m_currentStatement{ NodeKind::Unknown }
        , m_currentBasicBlock{ nullptr }
    {
    }

    bool LLVMCodeGenerator::generate()
    {
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
            case NodeKind::FunctionDefinitionStatement:
            {
                m_currentStatement = NodeKind::FunctionDefinitionStatement;
                generateFunctionDefinition((FunctionDefinitionStatement*)node);
                break;
            }
            case NodeKind::ReturnStatement:
            {
                m_currentStatement = NodeKind::ReturnStatement;
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

    void LLVMCodeGenerator::generateFunctionDefinition(FunctionDefinitionStatement* node) noexcept
    {
        const auto oldScope = m_currentScope;
        m_currentScope = Scope::Function;

        const auto nameExpression = node->nameExpression().get();
        const auto functionName = m_parseTree.tokens().getLexeme(nameExpression->nameToken());
        auto parametersNode = node->parametersNode().get();
        auto returnTypesNode = node->returnTypesNode().get();


        auto llvmFunction = m_module.getFunction(functionName);
        if (llvmFunction == nullptr)
        {
            auto llvmFunctionType = generateFunctionType(returnTypesNode, parametersNode);
            // TODO handle linkage types
            llvmFunction = llvm::Function::Create(llvmFunctionType, llvm::Function::ExternalLinkage, functionName, &m_module);
        }

        const auto& body = node->bodyNode();
        generateFunctionBody(body.get(), llvmFunction);

        m_currentScope = oldScope; // Reset the scope after generating the function definition
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
            case NodeKind::NumberLiteral:
            {
                m_currentStatement = NodeKind::NumberLiteral;
                return generateNumberLiteral((NumberLiteral*)node);
            }
            default:
            {
                TODO("Missing NodeKind!!");
                break;
            }
        }
    }

    llvm::Value* LLVMCodeGenerator::generateNumberLiteral(NumberLiteral* node) noexcept
    {
        auto& context = m_module.getContext();
        const auto literalType = node->type();
        const auto lexeme = m_parseTree.tokens().getLexeme(node->literalToken());

        if (literalType == Type::I32())
        {
            auto llvmReturnType = GetLLVMTypeForCaraType(literalType, context);
            auto caraValue = std::stoi(lexeme.data());
            auto returnValue = llvm::ConstantInt::get(llvmReturnType, caraValue);
            return returnValue;
        }
         
        TODO("Handle other literal types in generateNumberLiteral");
        return nullptr;
    }

    llvm::FunctionType* LLVMCodeGenerator::generateFunctionType(ReturnTypesNode* returnTypesNode, ParametersNode* parametersNode) noexcept
    {
        auto& context = m_module.getContext();
        
        llvm::Type* llvmReturnType = nullptr;
        
        const auto& returnTypes = returnTypesNode->returnTypes();
        const auto hasReturnTypes = !returnTypes.empty();

        if (!hasReturnTypes)
        {
            llvmReturnType = llvm::Type::getVoidTy(context);
        }
        else
        {
            if (returnTypes.size() != 1)
            {
                TODO("Implement multiple return types in CppCodeGenerator::generateFunctionSignature");
            }

            const auto caraReturnType = returnTypes[0]->type();
            llvmReturnType = GetLLVMTypeForCaraType(caraReturnType, context);
        }

        std::vector<llvm::Type*> llvmParameterTypes;
        const auto& parameters = parametersNode->parameters();
        for(const auto& parameter : parameters)
        {
            const auto caraParameterType = parameter->typeName()->type();
            auto llvmParameterType = GetLLVMTypeForCaraType(caraParameterType, context);
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

    bool generateLLVMModule(const ParseTree& parseTree, llvm::Module& module) noexcept
    {
        LLVMCodeGenerator generator{ parseTree, module };
        return generator.generate();
    }
}
