//#pragma once
//
//#include <Caracal/Defines.h>
//#include <Caracal/API.h>
//#include <Compiler/DiagnosticsBag.h>
//#include <Semantic/Parameter.h>
//#include <Semantic/Scope.h>
//#include <Semantic/Type.h>
//#include <Semantic/TypeCheckerOptions.h>
//#include <Semantic/TypeDatabase.h>
//#include <Semantic/TypedExpression.h>
//#include <Semantic/TypedFieldDefinitionNode.h>
//#include <Semantic/TypedMethodDefinitionStatement.h>
//#include <Semantic/TypedNode.h>
//#include <Semantic/TypedStatement.h>
//#include <Semantic/TypedTree.h>
//#include <Caracal/Syntax/AssignmentStatement.h>
//#include <Caracal/Syntax/BinaryExpression.h>
//#include <Caracal/Syntax/BoolLiteral.h>
//#include <Caracal/Syntax/DiscardLiteral.h>
//#include <Caracal/Syntax/EnumDefinitionStatement.h>
//#include <Caracal/Syntax/ExpressionStatement.h>
//#include <Caracal/Syntax/FunctionCallExpression.h>
//#include <Caracal/Syntax/FunctionDefinitionStatement.h>
//#include <Caracal/Syntax/GroupingExpression.h>
//#include <Caracal/Syntax/IfStatement.h>
//#include <Caracal/Syntax/MemberAccessExpression.h>
//#include <Caracal/Syntax/MethodDefinitionStatement.h>
//#include <Caracal/Syntax/NameExpression.h>
//#include <Caracal/Syntax/NumberLiteral.h>
//#include <Caracal/Syntax/ParseTree.h>
//#include <Caracal/Syntax/ReturnStatement.h>
//#include <Caracal/Syntax/TypeDefinitionStatement.h>
//#include <Caracal/Syntax/UnaryExpression.h>
//#include <Caracal/Syntax/WhileStatement.h>
//
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
//};
//
//CARACAL_API TypedTree TypeCheck(
//    const ParseTree& parseTree, 
//    const TypeCheckerOptions& options, 
//    TypeDatabase& typeDatabase, 
//    DiagnosticsBag& diagnostics) noexcept;
