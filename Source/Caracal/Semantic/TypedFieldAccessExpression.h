//#pragma once
//
//#include <Caracal/API.h>
//#include <Semantic/Type.h>
//#include <Semantic/TypedExpression.h>
//#include <Semantic/Field.h>
//
//class CARACAL_API TypedFieldAccessExpression : public TypedExpression
//{
//public:
//    TypedFieldAccessExpression(Type thisType, Field* field, Node* source);
//
//    [[nodiscard]] Field* field() const noexcept { return m_field; }
//    [[nodiscard]] QStringView fieldName() const noexcept { return m_field->name(); }
//    [[nodiscard]] Type thisType() const noexcept { return m_thisType; }
//
//private:
//    Field* m_field;
//    Type m_thisType;
//};
