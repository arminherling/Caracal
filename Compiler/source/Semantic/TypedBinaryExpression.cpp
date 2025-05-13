//#include "TypedBinaryExpression.h"
//
//TypedBinaryExpression::TypedBinaryExpression(
//    NodeKind kind,
//    Type type, 
//    TypedExpression* leftExpression, 
//    TypedExpression* rightExpression, 
//    Node* source)
//    : TypedExpression(source, kind, type)
//    , m_leftExpression{ leftExpression }
//    , m_rightExpression{ rightExpression }
//{
//}
//
//TypedAdditionExpression::TypedAdditionExpression(
//    Type type,
//    TypedExpression* leftExpression,
//    TypedExpression* rightExpression,
//    Node* source)
//    : TypedBinaryExpression(NodeKind::TypedAdditionExpression, type, leftExpression, rightExpression, source)
//{
//}
//
//TypedSubtractionExpression::TypedSubtractionExpression(
//    Type type,
//    TypedExpression* leftExpression,
//    TypedExpression* rightExpression,
//    Node* source)
//    : TypedBinaryExpression(NodeKind::TypedSubtractionExpression, type, leftExpression, rightExpression, source)
//{
//}
//
//TypedMultiplicationExpression::TypedMultiplicationExpression(
//    Type type,
//    TypedExpression* leftExpression,
//    TypedExpression* rightExpression,
//    Node* source)
//    : TypedBinaryExpression(NodeKind::TypedMultiplicationExpression, type, leftExpression, rightExpression, source)
//{
//}
//
//TypedDivisionExpression::TypedDivisionExpression(
//    Type type,
//    TypedExpression* leftExpression,
//    TypedExpression* rightExpression,
//    Node* source)
//    : TypedBinaryExpression(NodeKind::TypedDivisionExpression, type, leftExpression, rightExpression, source)
//{
//}
