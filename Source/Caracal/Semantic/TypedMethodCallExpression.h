//#pragma once
//
//#include <Caracal/API.h>
//#include <Semantic/Type.h>
//#include <Semantic/TypedExpression.h>
//#include <QList>
//
//class CARACAL_API TypedMethodCallExpression : public TypedExpression
//{
//public:
//    TypedMethodCallExpression(
//        QStringView name,
//        Type thisType,
//        Type functionType,
//        const QList<TypedExpression*>& arguments,
//        Node* source,
//        Type returnType);
//
//    [[nodiscard]] QStringView name() const noexcept { return m_name; }
//    [[nodiscard]] Type thisType() const noexcept { return m_thisType; }
//    [[nodiscard]] Type functionType() const noexcept { return m_functionType; }
//    [[nodiscard]] QList<TypedExpression*> arguments() const noexcept { return m_arguments; }
//
//private:
//    QStringView m_name;
//    Type m_thisType;
//    Type m_functionType;
//    QList<TypedExpression*> m_arguments;
//};
