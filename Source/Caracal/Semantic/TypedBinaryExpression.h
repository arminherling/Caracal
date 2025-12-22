//#pragma once
//
//#include <Caracal/API.h>
//#include <Semantic/Type.h>
//#include <Semantic/TypedExpression.h>
//
//class CARACAL_API TypedBinaryExpression : public TypedExpression
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
//class CARACAL_API TypedAdditionExpression : public TypedBinaryExpression
//{
//public:
//    TypedAdditionExpression(
//        Type type,
//        TypedExpression* leftExpression,
//        TypedExpression* rightExpression,
//        Node* source);
//};
//
//class CARACAL_API TypedSubtractionExpression : public TypedBinaryExpression
//{
//public:
//    TypedSubtractionExpression(
//        Type type,
//        TypedExpression* leftExpression,
//        TypedExpression* rightExpression,
//        Node* source);
//};
//
//class CARACAL_API TypedMultiplicationExpression : public TypedBinaryExpression
//{
//public:
//    TypedMultiplicationExpression(
//        Type type,
//        TypedExpression* leftExpression,
//        TypedExpression* rightExpression,
//        Node* source);
//};
//
//class CARACAL_API TypedDivisionExpression : public TypedBinaryExpression
//{
//public:
//    TypedDivisionExpression(
//        Type type,
//        TypedExpression* leftExpression,
//        TypedExpression* rightExpression,
//        Node* source);
//};
