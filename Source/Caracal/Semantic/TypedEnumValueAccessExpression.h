//#pragma once
//
//#include <Caracal/API.h>
//#include <Semantic/Type.h>
//#include <Semantic/TypedExpression.h>
//#include <Semantic/Field.h>
//
//class CARACAL_API TypedEnumValueAccessExpression : public TypedExpression
//{
//public:
//    TypedEnumValueAccessExpression(Type type, Field* field, Node* source);
//
//    [[nodiscard]] Field* field() const noexcept { return m_field; }
//    [[nodiscard]] QStringView fieldName() const noexcept { return m_field->name(); }
//
//private:
//    Field* m_field;
//};
