#pragma once

#include <Caracal/API.h>
#include <Caracal/Defines.h>
#include <Caracal/Debug/DiagnosticsBag.h>
#include <Caracal/Semantic/TypeCheckerOptions.h>
#include <Caracal/Semantic/Module.h>
#include <Caracal/Semantic/Parameter.h>
#include <Caracal/Semantic/Scope.h>
#include <Caracal/Syntax/ParseTree.h>
#include <Caracal/Syntax/ConstantDeclaration.h>
#include <Caracal/Syntax/FunctionDefinitionStatement.h>
#include <Caracal/Syntax/NumberLiteral.h>
#include <Caracal/Syntax/GroupingExpression.h>
#include <Caracal/Syntax/UnaryExpression.h>
#include <Caracal/Syntax/BinaryExpression.h>
#include <Caracal/Syntax/ReturnStatement.h>
#include <Caracal/Syntax/VariableDeclaration.h>
#include <Caracal/Syntax/NameExpression.h>
#include <Caracal/Syntax/IfStatement.h>
#include <Caracal/Syntax/WhileStatement.h>
#include <Caracal/Syntax/AssignmentStatement.h>
#include <Caracal/Syntax/FunctionCallExpression.h>
#include <Caracal/Syntax/ExpressionStatement.h>
#include <Caracal/Syntax/EnumDefinitionStatement.h>
#include <Caracal/Syntax/TypeDefinitionStatement.h>
#include <Caracal/Syntax/MethodDefinitionStatement.h>

namespace Caracal
{
    class CARACAL_API TypeChecker
    {
    public:
        TypeChecker(
            const std::vector<ParseTreeUPtr>& parseTrees,
            const TypeCheckerOptions& options,
            Module& module,
            DiagnosticsBag& diagnostics);

        CARACAL_DELETE_COPY_DELETE_MOVE(TypeChecker)

        bool typeCheck();
        
    private:
        void collectDeclarations();
        void typeCheckSignatures();

        void typeCheckStatement(Statement* statement);
        void typeCheckConstantDeclaration(ConstantDeclaration* statement);
        void typeCheckVariableDeclaration(VariableDeclaration* statement);

        void typeCheckExpressionStatement(ExpressionStatement* statement);
        void typeCheckAssignmentStatement(AssignmentStatement* statement);
        void typeCheckFunctionDefinitionStatement(FunctionDefinitionStatement* statement);
        void typeCheckEnumDefinitionStatement(EnumDefinitionStatement* statement);
        void typeCheckTypeDefinitionStatement(TypeDefinitionStatement* statement);
        void typeCheckMethodDefinitionStatement(MethodDefinitionStatement* statement);
        void typeCheckIfStatement(IfStatement* statement);
        void typeCheckWhileStatement(WhileStatement* statement);
        void typeCheckReturnStatement(ReturnStatement* statement);
            
        [[nodiscard]] Type typeCheckExpression(Expression* expression);
        [[nodiscard]] Type typeCheckGroupingExpression(GroupingExpression* expression);
        [[nodiscard]] Type typeCheckUnaryExpressionExpression(UnaryExpression* unaryExpression);
        [[nodiscard]] Type typeCheckBinaryExpressionExpression(BinaryExpression* binaryExpression);
        [[nodiscard]] Type typeCheckNameExpression(NameExpression* nameExpression);
        [[nodiscard]] Type typeCheckFunctionCallExpression(FunctionCallExpression* functionCallExpression);

        [[nodiscard]] Type typeCheckNumberLiteral(NumberLiteral* literal);

        [[nodiscard]] Type typeCheckTypeNameNode(TypeNameNode* typeNameNode);
        [[nodiscard]] std::vector<Parameter> typeCheckParametersNode(ParametersNode* parametersNode);
        [[nodiscard]] std::vector<Type> typeCheckReturnTypesNode(ReturnTypesNode* returnTypesNode);
        [[nodiscard]] std::vector<Type> typeCheckArgumentsNode(ArgumentsNode* argumentsNode);

        void typeCheckBlockNode(BlockNode* body);
        [[nodiscard]] i32 convertToI32(NumberLiteral* literal);
        
        void pushScope(ScopeKind kind);
        void popScope();
        [[nodiscard]] Scope* currentScope() const noexcept;
        
        const std::vector<ParseTreeUPtr>& m_parseTrees;
        const ParseTree* m_currentParseTree;
        TypeCheckerOptions m_options;
        Module& m_module;
        DiagnosticsBag& m_diagnostics;
        Type m_currentReturnType;
        std::vector<std::unique_ptr<Scope>> m_scopes;
    };

    // we modify the parse trees in place and add type information to the nodes
    CARACAL_API bool typeCheck(
        const std::vector<ParseTreeUPtr>& parseTrees, 
        const TypeCheckerOptions& options, 
        Module& module, 
        DiagnosticsBag& diagnostics) noexcept;
};
