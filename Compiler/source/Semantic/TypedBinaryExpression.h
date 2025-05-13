//#pragma once
//
//#include <Compiler/API.h>
//#include <Semantic/Type.h>
//#include <Semantic/TypedExpression.h>
//
//class COMPILER_API TypedBinaryExpression : public TypedExpression
//{
//public:
//    TypedBinaryExpression(
//        NodeKind kind,
//        Type type, 
//        TypedExpression* leftExpression, 
//        TypedExpression* rightExpression, 
//        Node* source);
//
//    [[nodiscard]] TypedExpression* leftExpression() const noexcept { return m_leftExpression; }
//    [[nodiscard]] TypedExpression* rightExpression() const noexcept { return m_rightExpression; }
//
//private:
//    TypedExpression* m_leftExpression;
//    TypedExpression* m_rightExpression;
//};
//
//class COMPILER_API TypedAdditionExpression : public TypedBinaryExpression
//{
//public:
//    TypedAdditionExpression(
//        Type type,
//        TypedExpression* leftExpression,
//        TypedExpression* rightExpression,
//        Node* source);
//};
//
//class COMPILER_API TypedSubtractionExpression : public TypedBinaryExpression
//{
//public:
//    TypedSubtractionExpression(
//        Type type,
//        TypedExpression* leftExpression,
//        TypedExpression* rightExpression,
//        Node* source);
//};
//
//class COMPILER_API TypedMultiplicationExpression : public TypedBinaryExpression
//{
//public:
//    TypedMultiplicationExpression(
//        Type type,
//        TypedExpression* leftExpression,
//        TypedExpression* rightExpression,
//        Node* source);
//};
//
//class COMPILER_API TypedDivisionExpression : public TypedBinaryExpression
//{
//public:
//    TypedDivisionExpression(
//        Type type,
//        TypedExpression* leftExpression,
//        TypedExpression* rightExpression,
//        Node* source);
//};
