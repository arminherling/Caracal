//#pragma once
//
//#include <Compiler/API.h>
//#include <Debug/BasePrinter.h>
//#include <Defines.h>
//#include <Semantic/BoolValue.h>
//#include <Semantic/Discard.h>
//#include <Semantic/I32Value.h>
//#include <Semantic/Parameter.h>
//#include <Semantic/TypedAssignmentStatement.h>
//#include <Semantic/TypeDatabase.h>
//#include <Semantic/TypedBinaryExpression.h>
//#include <Semantic/TypedConstant.h>
//#include <Semantic/TypedEnumDefinitionStatement.h>
//#include <Semantic/TypedEnumValueAccessExpression.h>
//#include <Semantic/TypedExpressionStatement.h>
//#include <Semantic/TypedFieldAccessExpression.h>
//#include <Semantic/TypedFunctionCallExpression.h>
//#include <Semantic/TypedFunctionDefinitionStatement.h>
//#include <Semantic/TypedIfStatement.h>
//#include <Semantic/TypedMethodCallExpression.h>
//#include <Semantic/TypedMethodDefinitionStatement.h>
//#include <Semantic/TypedNegationExpression.h>
//#include <Semantic/TypedNode.h>
//#include <Semantic/TypedReferenceOfExpression.h>
//#include <Semantic/TypedReturnStatement.h>
//#include <Semantic/TypedTree.h>
//#include <Semantic/TypedTypeDefinitionStatement.h>
//#include <Semantic/TypedVariable.h>
//#include <Semantic/TypedWhileStatement.h>
//#include <Semantic/U8Value.h>
//
//class COMPILER_API TypedTreePrinter : public BasePrinter
//{
//public:
//    TypedTreePrinter(TypedTree& typedTree, const TypeDatabase& typeDatabase, i32 indentation = 4);
//
//    [[nodiscard]] QString PrettyPrint();
//
//private:
//    void PrettyPrintNode(TypedNode* node);
//
//    void PrettyPrintTypedAssignmentStatement(TypedAssignmentStatement* statement);
//    void PrettyPrintTypedExpressionStatement(TypedExpressionStatement* statement);
//    void PrettyPrintTypedEnumDefinitionStatement(TypedEnumDefinitionStatement* statement);
//    void PrettyPrintTypedTypeDefinitionStatement(TypedTypeDefinitionStatement* statement);
//    void PrettyPrintTypedFunctionDefinitionStatement(TypedFunctionDefinitionStatement* statement);
//    void PrettyPrintTypedMethodDefinitionStatement(TypedMethodDefinitionStatement* statement);
//    void PrettyPrintTypedIfStatement(TypedIfStatement* statement);
//    void PrettyPrintTypedWhileStatement(TypedWhileStatement* statement);
//    void PrettyPrintTypedReturnStatement(TypedReturnStatement* statement);
//    void PrettyPrintTypedReferenceOfExpression(TypedReferenceOfExpression* expression);
//    void PrettyPrintTypedNegationExpression(TypedNegationExpression* expression);
//    void PrettyPrintTypedEnumValueAccessExpression(TypedEnumValueAccessExpression* expression);
//    void PrettyPrintTypedFieldAccessExpression(TypedFieldAccessExpression* expression);
//    void PrettyPrintTypedBinaryExpression(TypedBinaryExpression* expression);
//    void PrettyPrintTypedConstant(TypedConstant* constant);
//    void PrettyPrintTypedVariable(TypedVariable* variable);
//    void PrettyPrintParameter(Parameter* parameter);
//    void PrettyPrintTypedFunctionCallExpression(TypedFunctionCallExpression* functionCall);
//    void PrettyPrintTypedMethodCallExpression(TypedMethodCallExpression* methodCall);
//    void PrettyPrintDiscard(Discard* discard);
//    void PrettyPrintBoolValue(BoolValue* value);
//    void PrettyPrintU8Value(U8Value* value);
//    void PrettyPrintI32Value(I32Value* value);
//
//    void PrettyPrintTypedFieldDefinitionNode(TypedFieldDefinitionNode* field);
//    void PrettyPrintTypedArgumentsNode();
//    [[nodiscard]] QString PrettyPrintType(Type type, Type scope = Type::Undefined()) noexcept;
//    [[nodiscard]] QString PrettyPrintTypeName(Type type) noexcept;
//
//    TypedTree& m_typedTree;
//    TypeDatabase m_typeDatabase;
//};
