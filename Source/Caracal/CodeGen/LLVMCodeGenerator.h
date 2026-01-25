#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/ParseTree.h>
#include <Caracal/Syntax/ConstantDeclaration.h>
#include <Caracal/Syntax/FunctionDefinitionStatement.h>
#include <Caracal/Syntax/ReturnStatement.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/BoolLiteral.h>
#include <Caracal/Syntax/NumberLiteral.h>
#include <Caracal/Syntax/StringLiteral.h>
#include <Caracal/Syntax/FunctionCallExpression.h>
#include <Caracal/Syntax/NameExpression.h>
#include <Caracal/Syntax/ExpressionStatement.h>
#include <Caracal/Semantic/TypeDatabase.h>

// forward declare so that we keep the headers clean and dont need to link llvm in the tests
namespace llvm {
    class Module;
    class Value;
    class Function;
    class FunctionType;
    class BasicBlock;
    class Constant;
    class GlobalValue;
}

namespace Caracal
{
    class LLVMCodeGenerator
    {
    public:
        LLVMCodeGenerator(
            const ParseTree& parseTree, 
            TypeDatabase& typeDatabase, 
            llvm::Module& module);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(LLVMCodeGenerator)

        [[nodiscard]] bool generate();
    
    private:
        enum class Scope
        {
            Global,
            Function
        };

        void generateNode(Node* node) noexcept;
        void generateConstantDeclaration(ConstantDeclaration* node) noexcept;
        void generateFunctionDefinition(FunctionDefinitionStatement* node) noexcept;
        void generateExpressionStatement(ExpressionStatement* node) noexcept;
        void generateReturnStatement(ReturnStatement* node) noexcept;

        llvm::Value* generateExpression(Expression* node) noexcept;
        llvm::Value* generateFunctionCallExpression(FunctionCallExpression* node) noexcept;
        llvm::Value* generateNameExpression(NameExpression* node) noexcept; 
        llvm::Value* generateBoolLiteral(BoolLiteral* node) noexcept;
        llvm::Value* generateNumberLiteral(NumberLiteral* node) noexcept;
        llvm::Value* generateStringLiteral(StringLiteral* node) noexcept;

        llvm::FunctionType* generateFunctionType(FunctionDefinition& functionDefinition) noexcept;
        void generateFunctionBody(BlockNode* body, llvm::Function* llvmFunction) noexcept;

        void setupPrintfFunctionDeclaration() noexcept;
        llvm::GlobalValue* createGlobalValue(const std::string& name, llvm::Constant* constant, bool isConst) noexcept;

    private:
        const ParseTree& m_parseTree;
        TypeDatabase& m_typeDatabase;
        llvm::Module& m_module;
        Scope m_currentScope;
        llvm::BasicBlock* m_currentBasicBlock;
    };

    bool generateLLVMModule(const ParseTree& parseTree, TypeDatabase& typeDatabase, llvm::Module& module) noexcept;
}
