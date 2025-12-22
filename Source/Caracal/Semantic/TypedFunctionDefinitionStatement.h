//#pragma once
//
//#include <Caracal/API.h>
//#include <Semantic/Parameter.h>
//#include <Semantic/Type.h>
//#include <Semantic/TypedFieldDefinitionNode.h>
//#include <Semantic/TypedStatement.h>
//#include <QList>
//
//class CARACAL_API TypedFunctionDefinitionStatement : public TypedStatement
//{
//public:
//    TypedFunctionDefinitionStatement(
//        QStringView name,
//        Type type,
//        const QList<Parameter*>& parameters,
//        Type returnType,
//        const QList<TypedStatement*>& statements,
//        Node* source);
//
//    [[nodiscard]] QStringView name() const noexcept { return m_name; }
//    [[nodiscard]] const QList<Parameter*>& parameters() const noexcept { return m_parameters; }
//    [[nodiscard]] Type returnType() const noexcept { return m_returnType; }
//    [[nodiscard]] const QList<TypedStatement*>& body() const noexcept { return m_statements; }
//
//private:
//    QStringView m_name;
//    QList<Parameter*> m_parameters;
//    Type m_returnType;
//    QList<TypedStatement*> m_statements;
//};
