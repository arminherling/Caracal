//#pragma once
//
//#include <Compiler/API.h>
//#include <Semantic/Type.h>
//#include <Semantic/TypedFieldDefinitionNode.h>
//#include <Semantic/TypedMethodDefinitionStatement.h>
//#include <Semantic/TypedStatement.h>
//
//#include <QList>
//
//class COMPILER_API TypedTypeDefinitionStatement : public TypedStatement
//{
//public:
//    TypedTypeDefinitionStatement(
//        QStringView name,
//        Type type,
//        const QList<TypedFieldDefinitionNode*>& fields,
//        const QList<TypedMethodDefinitionStatement*>& methods,
//        Node* source);
//
//    [[nodiscard]] QStringView name() const noexcept { return m_name; }
//    [[nodiscard]] QList<TypedFieldDefinitionNode*> fields() const noexcept { return m_fields; }
//    [[nodiscard]] QList<TypedMethodDefinitionStatement*> methods() const noexcept { return m_methods; }
//
//private:
//    QStringView m_name;
//    QList<TypedFieldDefinitionNode*> m_fields;
//    QList<TypedMethodDefinitionStatement*> m_methods;
//};
