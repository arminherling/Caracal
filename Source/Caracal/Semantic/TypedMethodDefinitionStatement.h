//#pragma once
//
//#include <Caracal/API.h>
//#include <Semantic/Parameter.h>
//#include <Semantic/Type.h>
//#include <Semantic/TypedFieldDefinitionNode.h>
//#include <Semantic/TypedStatement.h>
//#include <QList>
//
//class CARACAL_API TypedMethodDefinitionStatement : public TypedStatement
//{
//public:
//    TypedMethodDefinitionStatement(
//        QStringView name,
//        Type parentType,
//        Type methodType,
//        const QList<Parameter*>& parameters,
//        Type returnType,
//        const QList<TypedStatement*>& statements,
//        Node* source);
//
//    [[nodiscard]] QStringView name() const noexcept { return m_name; }
//    [[nodiscard]] Type parentType() const noexcept { return m_parentType; }
//    [[nodiscard]] const QList<Parameter*>& parameters() const noexcept { return m_parameters; }
//    [[nodiscard]] Type returnType() const noexcept { return m_returnType; }
//    [[nodiscard]] const QList<TypedStatement*>& body() const noexcept { return m_statements; }
//
//private:
//    QStringView m_name;
//    Type m_parentType;
//    QList<Parameter*> m_parameters;
//    Type m_returnType;
//    QList<TypedStatement*> m_statements;
//};
