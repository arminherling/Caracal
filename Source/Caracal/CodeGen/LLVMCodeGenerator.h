#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/ParseTree.h>
#include <Caracal/Syntax/ConstantDeclaration.h>
#include <Caracal/Syntax/VariableDeclaration.h>
#include <Caracal/Syntax/AssignmentStatement.h>
#include <Caracal/Syntax/FunctionDefinitionStatement.h>
#include <Caracal/Syntax/IfStatement.h>
#include <Caracal/Syntax/WhileStatement.h>
#include <Caracal/Syntax/ReturnStatement.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/BoolLiteral.h>
#include <Caracal/Syntax/NumberLiteral.h>
#include <Caracal/Syntax/StringLiteral.h>
#include <Caracal/Syntax/BinaryExpression.h>
#include <Caracal/Syntax/FunctionCallExpression.h>
#include <Caracal/Syntax/NameExpression.h>
#include <Caracal/Syntax/ExpressionStatement.h>
#include <Caracal/Semantic/Module.h>
#include <Caracal/CodeGen/LLVMScope.h>
#include <Caracal/Syntax/TypeDefinitionStatement.h>
#include <Caracal/Syntax/UnaryExpression.h>

// forward declare so that we keep the headers clean and dont need to link llvm in the tests
namespace llvm {
    class Module;
    class Value;
    class Function;
    class FunctionType;
    class BasicBlock;
    class Constant;
    class GlobalValue;
    class Type;
    class IRBuilderBase;
}

namespace Caracal
{
    class LLVMCodeGenerator
    {
    public:
        LLVMCodeGenerator(
            const ParseTree& parseTree, 
            Module& caracalModule, 
            llvm::Module& llvmModule);

        CARACAL_DELETE_COPY_DELETE_MOVE(LLVMCodeGenerator)

        [[nodiscard]] bool generate();
    
    private:
        void generateNode(Node* node) noexcept;
        void generateConstantDeclaration(ConstantDeclaration* node) noexcept;
        void generateVariableDeclaration(VariableDeclaration* node) noexcept;
        void generateExpressionStatement(ExpressionStatement* node) noexcept;
        void generateAssignmentStatement(AssignmentStatement* node) noexcept;
        void generateFunctionDefinition(FunctionDefinitionStatement* node) noexcept;
        void generateFunction(Type functionType, BlockNode* body) noexcept;
        void generateTypeDefinition(TypeDefinitionStatement* node) noexcept;
        void generateIfStatement(IfStatement* node) noexcept;
        void generateWhileStatement(WhileStatement* node) noexcept;
        void generateBreakStatement() noexcept;
        void generateSkipStatement() noexcept;
        void generateReturnStatement(ReturnStatement* node) noexcept;

        llvm::Value* generateExpression(Expression* node) noexcept;
        llvm::Value* generateBinaryExpression(BinaryExpression* node) noexcept;
        llvm::Value* generateUnaryExpression(UnaryExpression* node) noexcept;
        llvm::Value* generateNameExpression(NameExpression* node) noexcept; 
        llvm::Value* generateFunctionCallExpression(FunctionCallExpression* node) noexcept;
        llvm::Value* generateBoolLiteral(BoolLiteral* node) noexcept;
        llvm::Value* generateNumberLiteral(NumberLiteral* node) noexcept;
        llvm::Value* generateStringLiteral(StringLiteral* node) noexcept;

        void generateBlockNode(BlockNode* body) noexcept;
        void declareFunction(const FunctionDefinition& functionDefinition) noexcept;
        void declareAllFunctions() noexcept;
        llvm::GlobalValue* createGlobalValue(const std::string& name, llvm::Constant* constant, bool isConst) noexcept;
        llvm::Value* createLocalValue(const std::string& name, llvm::Type* type) noexcept;
        llvm::Function* getFunctionDeclaration(const FunctionDefinition& functionDefinition);
        llvm::FunctionType* buildFunctionType(const FunctionDefinition& functionDefinition) noexcept;
        llvm::Value* dereferenceIfNeeded(Expression* expr, llvm::Value* val, llvm::Type* desiredTy) noexcept;
        llvm::Value* getPointerForAssignment(Expression* expr, llvm::Value* val) noexcept;

        void pushScope();
        void popScope();
        [[nodiscard]] LLVMScope* currentScope() const noexcept;

    private:
        const ParseTree& m_parseTree;
        Module& m_caracalModule;
        llvm::Module& m_llvmModule;
        llvm::Function* m_currentFunction;
        llvm::BasicBlock* m_currentConditionBlock;
        llvm::BasicBlock* m_currentEndBlock;
        std::unique_ptr<llvm::IRBuilderBase> m_irBuilder;
        std::vector<std::unique_ptr<LLVMScope>> m_scopes;
    };

    bool generateLLVMModule(const ParseTree& parseTree, Module& caracalModule, llvm::Module& llvmModule) noexcept;
}
