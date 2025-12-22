//#pragma once
//
//#include <Caracal/API.h>
//#include <Semantic/Type.h>
//#include <Semantic/TypedExpression.h>
//#include <QStringView>
//
//class CARACAL_API Field
//{
//public:
//    Field(Type type, QStringView name, TypedExpression* expression);
//
//    [[nodiscard]] Type type() const noexcept { return m_type; }
//    [[nodiscard]] QStringView name() const noexcept { return m_name; }
//    [[nodiscard]] TypedExpression* value() const noexcept { return m_expression; }
//
//private:
//    Type m_type; 
//    QStringView m_name; 
//    TypedExpression* m_expression;
//};
//
//CARACAL_API bool operator==(Field lhs, Field rhs) noexcept;
//CARACAL_API bool operator!=(Field lhs, Field rhs) noexcept;
