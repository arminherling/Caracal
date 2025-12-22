//#pragma once
//
//#include <Caracal/API.h>
//#include <Semantic/Type.h>
//#include <Semantic/TypedExpression.h>
//#include <Semantic/TypedStatement.h>
//#include <QList>
//
//class CARACAL_API TypedWhileStatement : public TypedStatement
//{
//public:
//    TypedWhileStatement(
//        TypedExpression* condition,
//        const QList<TypedStatement*>& body,
//        Node* source,
//        Type type);
//
//    [[nodiscard]] TypedExpression* condition() const noexcept { return m_condition; }
//    [[nodiscard]] const QList<TypedStatement*>& body() const noexcept { return m_body; }
//
//private:
//    TypedExpression* m_condition;
//    QList<TypedStatement*> m_body;
//};
