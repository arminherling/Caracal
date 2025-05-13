//#pragma once
//
//#include <Compiler/API.h>
//#include <Semantic/Type.h>
//#include <Semantic/TypedExpression.h>
//#include <Semantic/TypedStatement.h>
//
//class COMPILER_API TypedAssignmentStatement : public TypedStatement
//{
//public:
//    TypedAssignmentStatement(
//        TypedExpression* leftExpression,
//        TypedExpression* rightExpression,
//        Node* source,
//        Type type);
//
//    [[nodiscard]] TypedExpression* leftExpression() const noexcept { return m_leftExpression; }
//    [[nodiscard]] TypedExpression* rightExpression() const noexcept { return m_rightExpression; }
//
//private:
//    TypedExpression* m_leftExpression;
//    TypedExpression* m_rightExpression;
//};
