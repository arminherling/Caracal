//#pragma once
//
//#include <Caracal/Defines.h>
//#include <Semantic/Field.h>
//#include <Semantic/Parameter.h>
//#include <Semantic/Type.h>
//#include <Semantic/TypedExpression.h>
//#include <Semantic/FunctionDefinition.h>
//#include <QList>
//#include <QString>
//
//class TypeDefinition
//{
//public:
//    TypeDefinition(Type type, const QString& name);
//
//    [[nodiscard]] Type type() const noexcept { return m_type; }
//    [[nodiscard]] QString name() const noexcept { return m_name; }
//    [[nodiscard]] Field* getFieldByName(QStringView fieldName) const noexcept;
//    [[nodiscard]] FunctionDefinition& getFunctionDefinition(Type type) noexcept;
//    [[nodiscard]] FunctionDefinition& getFunctionDefinitionByName(QStringView fieldName) noexcept;
//
//    void addField(Type type, QStringView name, TypedExpression* expression) noexcept;
//    void addFunction(Type type, QStringView name) noexcept;
//
//private:
//    Type m_type;
//    QString m_name;
//    std::unordered_map<QString, Field*> m_fields;
//    std::unordered_map<QString, Type> m_functionNames;
//    std::unordered_map<i32, FunctionDefinition> m_functions;
//};
