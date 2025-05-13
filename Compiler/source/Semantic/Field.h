//#pragma once
//
//#include <Compiler/API.h>
//#include <Semantic/Type.h>
//#include <Semantic/TypedExpression.h>
//#include <QStringView>
//
//class COMPILER_API Field
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
//COMPILER_API bool operator==(Field lhs, Field rhs) noexcept;
//COMPILER_API bool operator!=(Field lhs, Field rhs) noexcept;
