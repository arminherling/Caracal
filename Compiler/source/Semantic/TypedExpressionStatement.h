//#pragma once
//
//#include <Compiler/API.h>
//#include <Semantic/Type.h>
//#include <Semantic/TypedExpression.h>
//#include <Semantic/TypedStatement.h>
//
//class COMPILER_API TypedExpressionStatement : public TypedStatement
//{
//public:
//    TypedExpressionStatement(
//        TypedExpression* expression,
//        Node* source,
//        Type type);
//
//    [[nodiscard]] TypedExpression* expression() const noexcept { return m_expression; }
//
//private:
//    TypedExpression* m_expression;
//};
