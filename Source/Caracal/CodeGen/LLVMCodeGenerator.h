#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/ParseTree.h>
#include <Caracal/Syntax/FunctionDefinitionStatement.h>
#include <Caracal/Syntax/ReturnStatement.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/NumberLiteral.h>
#include <llvm/IR/Module.h>

namespace Caracal
{
    class CARACAL_API LLVMCodeGenerator
    {
    public:
        LLVMCodeGenerator(const ParseTree& parseTree, llvm::Module& module);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(LLVMCodeGenerator)

        [[nodiscard]] bool generate();
    
    private:
        enum class Scope
        {
            Global,
            Function
        };

        void generateNode(Node* node) noexcept;
        void generateFunctionDefinition(FunctionDefinitionStatement* node) noexcept;
        void generateReturnStatement(ReturnStatement* node) noexcept;

        llvm::Value* generateExpression(Expression* node) noexcept;
        llvm::Value* generateNumberLiteral(NumberLiteral* node) noexcept;

        llvm::FunctionType* generateFunctionType(ReturnTypesNode* returnTypesNode, ParametersNode* parametersNode) noexcept;
        void generateFunctionBody(BlockNode* body, llvm::Function* llvmFunction) noexcept;

    private:
        const ParseTree& m_parseTree;
        llvm::Module& m_module;
        Scope m_currentScope;
        NodeKind m_currentStatement;
        llvm::BasicBlock* m_currentBasicBlock;
    };

    CARACAL_API bool generateLLVMModule(const ParseTree& parseTree, llvm::Module& module) noexcept;
}
