//#pragma once
//
//#include <Compiler/API.h>
//#include <Semantic/Type.h>
//#include <Semantic/TypedExpression.h>
//#include <QList>
//
//class COMPILER_API TypedFunctionCallExpression : public TypedExpression
//{
//public:
//    TypedFunctionCallExpression(
//        QStringView name, 
//        Type functionType, 
//        const QList<TypedExpression*>& arguments, 
//        Node* source, 
//        Type type);
//
//    [[nodiscard]] QStringView name() const noexcept { return m_name; }
//    [[nodiscard]] Type functionType() const noexcept { return m_functionType; }
//    [[nodiscard]] QList<TypedExpression*> arguments() const noexcept { return m_arguments; }
//
//private:
//    QStringView m_name;
//    Type m_functionType;
//    QList<TypedExpression*> m_arguments;
//};
