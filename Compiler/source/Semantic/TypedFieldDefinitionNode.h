//#pragma once
//
//#include <Compiler/API.h>
//#include <Syntax/Node.h>
//#include <Semantic/TypedExpression.h>
//
//class COMPILER_API TypedFieldDefinitionNode : public Node
//{
//public:
//    TypedFieldDefinitionNode(QStringView name, Type type, TypedExpression* value);
//
//    [[nodiscard]] QStringView name() const noexcept { return m_name; }
//    [[nodiscard]] TypedExpression* value() const noexcept { return m_value; }
//    [[nodiscard]] Type valueType() const noexcept { return m_type; }
//
//private:
//    QStringView m_name;
//    Type m_type;
//    TypedExpression* m_value;
//};
