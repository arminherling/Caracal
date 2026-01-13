#pragma once

#include <Caracal/API.h>
#include <Caracal/Defines.h>
#include <Caracal/Debug/DiagnosticsBag.h>
#include <Caracal/Syntax/ParseTree.h>
#include <Caracal/Semantic/TypeCheckerOptions.h>
#include <Caracal/Semantic/TypeDatabase.h>

namespace Caracal
{
//class CARACAL_API TypeChecker
//{
//public:
//    TypeChecker(const TypeChecker&) = delete;
//
//    TypeChecker(
//        const ParseTree& parseTree, 
//        const TypeCheckerOptions& options, 
//        TypeDatabase& typeDatabase, 
//        DiagnosticsBag& diagnostics);
//
//    TypedTree typeCheck();
//
//private:
//    [[nodiscard]] TypedStatement* typeCheckStatement(Statement* statement);
//    [[nodiscard]] TypedExpression* typeCheckExpression(Expression* expression);
//
//    [[nodiscard]] TypedStatement* typeCheckAssignmentStatement(AssignmentStatement* statement);
//    [[nodiscard]] TypedStatement* typeCheckExpressionStatement(ExpressionStatement* statement);
//    [[nodiscard]] TypedStatement* typeCheckEnumDefinitionStatement(EnumDefinitionStatement* statement);
//    [[nodiscard]] TypedStatement* typeCheckTypeDefinitionStatement(TypeDefinitionStatement* statement);
//    [[nodiscard]] TypedStatement* typeCheckFunctionDefinitionStatement(FunctionDefinitionStatement* statement);
//    [[nodiscard]] TypedMethodDefinitionStatement* typeCheckTypeMethodDefinitionStatement(Type newRefType, Type newType, MethodDefinitionStatement* statement);
//    [[nodiscard]] TypedStatement* typeCheckIfStatement(IfStatement* statement);
//    [[nodiscard]] TypedStatement* typeCheckWhileStatement(WhileStatement* statement);
//    [[nodiscard]] TypedStatement* typeCheckReturnStatement(ReturnStatement* statement);
//    [[nodiscard]] QList<TypedFieldDefinitionNode*> typeCheckEnumFieldDefinitionNodes(
//        Type newType, 
//        Type baseType, 
//        const QList<EnumFieldDefinitionStatement*>& fieldDefinitions);
//    [[nodiscard]] QList<TypedFieldDefinitionNode*> typeCheckTypeFieldDefinitionNodes(Type newType, BlockNode* body);
//    [[nodiscard]] QList<TypedMethodDefinitionStatement*> typeCheckTypeMethodDefinitions(Type newRefType, Type newType, BlockNode* body);
//    [[nodiscard]] QList<Parameter*> typeCheckFunctionParameters(ParametersNode* parametersNode);
//    [[nodiscard]] std::tuple<QList<TypedStatement*>, Type> typeCheckFunctionBodyNode(BlockNode* body);
//    [[nodiscard]] TypedExpression* typeCheckUnaryExpressionExpression(UnaryExpression* unaryExpression);
//    [[nodiscard]] TypedExpression* typeCheckBinaryExpressionExpression(BinaryExpression* binaryExpression);
//    [[nodiscard]] TypedExpression* typeCheckFunctionCallExpression(FunctionCallExpression* functionCallExpression); 
//    [[nodiscard]] QList<TypedExpression*> typeCheckFunctionCallArguments(ArgumentsNode* argumentsNode);
//    [[nodiscard]] TypedExpression* typeCheckNameExpression(NameExpression* expression);
//    [[nodiscard]] TypedExpression* typeCheckGroupingExpression(GroupingExpression* expression);
//    [[nodiscard]] TypedExpression* typeCheckMemberAccessExpression(MemberAccessExpression* expression);
//    [[nodiscard]] TypedExpression* typeCheckDiscardLiteral(DiscardLiteral* literal);
//    [[nodiscard]] TypedExpression* typeCheckBoolLiteral(BoolLiteral* literal);
//    [[nodiscard]] TypedExpression* typeCheckNumberLiteral(NumberLiteral* literal);
//
//    [[nodiscard]] Type inferType(TypedNode* node);
//    [[nodiscard]] Type convertTypeNameToType(const TypeName& typeName);
//    [[nodiscard]] std::tuple<TypedExpression*, i32> convertValueToTypedLiteral(QStringView literal, Type type, Node* source);
//    [[nodiscard]] std::tuple<TypedExpression*, i32> convertValueToTypedLiteral(i32 value, Type type, Node* source);
//
//    void pushScope(ScopeKind kind);
//    void popScope();
//    [[nodiscard]] Scope* currentScope() const noexcept;
//
//    ParseTree m_parseTree;
//    TypeCheckerOptions m_options;
//    std::vector<std::unique_ptr<Scope>> m_scopes;
//    TypeDatabase& m_typeDatabase;
//    DiagnosticsBag& m_diagnostics;

    // we modify the parse tree in place and add type information to the nodes
    CARACAL_API bool typeCheck(
        const ParseTree& parseTree, 
        const TypeCheckerOptions& options, 
        TypeDatabase& typeDatabase, 
        DiagnosticsBag& diagnostics) noexcept;
};

