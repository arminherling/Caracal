#pragma once

#include <Caracal/API.h>
#include <Caracal/Defines.h>
#include <Caracal/Debug/DiagnosticsBag.h>
#include <Caracal/Semantic/TypeCheckerOptions.h>
#include <Caracal/Semantic/TypeDatabase.h>
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
#include <Caracal/Semantic/Scope.h>

namespace Caracal
{
    class CARACAL_API TypeChecker
    {
    public:
        TypeChecker(
            ParseTree& parseTree,
            const TypeCheckerOptions& options,
            TypeDatabase& typeDatabase,
            DiagnosticsBag& diagnostics);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(TypeChecker)

        bool typeCheck();
        
        private:
            void typeCheckStatement(Statement* statement);
            void typeCheckConstantDeclaration(ConstantDeclaration* statement);
            void typeCheckVariableDeclaration(VariableDeclaration* statement);
            void typeCheckFunctionDefinitionStatement(FunctionDefinitionStatement* statement);
            void typeCheckIfStatement(IfStatement* statement);
            void typeCheckWhileStatement(WhileStatement* statement);
            void typeCheckReturnStatement(ReturnStatement* statement);
            void typeCheckBlockNode(BlockNode* body);

            [[nodiscard]] Type typeCheckExpression(Expression* expression);
            [[nodiscard]] Type typeCheckGroupingExpression(GroupingExpression* expression);
            [[nodiscard]] Type typeCheckUnaryExpressionExpression(UnaryExpression* unaryExpression);
            [[nodiscard]] Type typeCheckBinaryExpressionExpression(BinaryExpression* binaryExpression);
            [[nodiscard]] Type typeCheckNameExpression(NameExpression* nameExpression);
            [[nodiscard]] Type typeCheckNumberLiteral(NumberLiteral* literal);
            [[nodiscard]] Type typeCheckTypeNameNode(TypeNameNode* typeNameNode);
            
            void typeCheckReturnTypesNode(ReturnTypesNode* returnTypesNode);

        //    [[nodiscard]] TypedStatement* typeCheckExpressionStatement(ExpressionStatement* statement);
        //    [[nodiscard]] TypedStatement* typeCheckEnumDefinitionStatement(EnumDefinitionStatement* statement);
        //    [[nodiscard]] TypedStatement* typeCheckTypeDefinitionStatement(TypeDefinitionStatement* statement);
        //    [[nodiscard]] TypedMethodDefinitionStatement* typeCheckTypeMethodDefinitionStatement(Type newRefType, Type newType, MethodDefinitionStatement* statement);
        //    [[nodiscard]] TypedStatement* typeCheckIfStatement(IfStatement* statement);
        //    [[nodiscard]] TypedStatement* typeCheckWhileStatement(WhileStatement* statement);
        //    [[nodiscard]] QList<TypedFieldDefinitionNode*> typeCheckEnumFieldDefinitionNodes(
        //        Type newType, 
        //        Type baseType, 
        //        const QList<EnumFieldDefinitionStatement*>& fieldDefinitions);
        //    [[nodiscard]] QList<TypedFieldDefinitionNode*> typeCheckTypeFieldDefinitionNodes(Type newType, BlockNode* body);
        //    [[nodiscard]] QList<TypedMethodDefinitionStatement*> typeCheckTypeMethodDefinitions(Type newRefType, Type newType, BlockNode* body);
        //    [[nodiscard]] QList<Parameter*> typeCheckFunctionParameters(ParametersNode* parametersNode);
        //    [[nodiscard]] TypedExpression* typeCheckFunctionCallExpression(FunctionCallExpression* functionCallExpression); 
        //    [[nodiscard]] QList<TypedExpression*> typeCheckFunctionCallArguments(ArgumentsNode* argumentsNode);
        //    [[nodiscard]] TypedExpression* typeCheckNameExpression(NameExpression* expression);
        //    [[nodiscard]] TypedExpression* typeCheckMemberAccessExpression(MemberAccessExpression* expression);
        //    [[nodiscard]] TypedExpression* typeCheckDiscardLiteral(DiscardLiteral* literal);
        //    [[nodiscard]] TypedExpression* typeCheckBoolLiteral(BoolLiteral* literal);
        //
        //    [[nodiscard]] Type inferType(TypedNode* node);
        //    [[nodiscard]] Type convertTypeNameToType(const TypeName& typeName);
        //    [[nodiscard]] std::tuple<TypedExpression*, i32> convertValueToTypedLiteral(QStringView literal, Type type, Node* source);
        //    [[nodiscard]] std::tuple<TypedExpression*, i32> convertValueToTypedLiteral(i32 value, Type type, Node* source);
        
            void pushScope(ScopeKind kind);
            void popScope();
            [[nodiscard]] Scope* currentScope() const noexcept;
        
            ParseTree& m_parseTree;
            TypeCheckerOptions m_options;
            TypeDatabase& m_typeDatabase;
            DiagnosticsBag& m_diagnostics;
            Type m_currentReturnType;
            std::vector<std::unique_ptr<Scope>> m_scopes;
    };

    // we modify the parse tree in place and add type information to the nodes
    CARACAL_API bool typeCheck(
        ParseTree& parseTree, 
        const TypeCheckerOptions& options, 
        TypeDatabase& typeDatabase, 
        DiagnosticsBag& diagnostics) noexcept;
};

