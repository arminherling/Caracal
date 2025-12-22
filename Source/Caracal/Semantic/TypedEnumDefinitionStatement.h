//#pragma once
//
//#include <Caracal/API.h>
//#include <Semantic/Type.h>
//#include <Semantic/TypedStatement.h>
//#include <Semantic/TypedFieldDefinitionNode.h>
//#include <QList>
//
//class CARACAL_API TypedEnumDefinitionStatement : public TypedStatement
//{
//public:
//    TypedEnumDefinitionStatement(
//        QStringView name,
//        Type type,
//        Type baseType,
//        const QList<TypedFieldDefinitionNode*>& fields,
//        Node* source);
//
//    [[nodiscard]] QStringView name() const noexcept { return m_name; }
//    [[nodiscard]] Type baseType() const noexcept { return m_baseType; }
//    [[nodiscard]] QList<TypedFieldDefinitionNode*> fields() const noexcept { return m_fields; }
//
//private:
//    QStringView m_name;
//    Type m_baseType;
//    QList<TypedFieldDefinitionNode*> m_fields;
//};
