//#include "TypeDefinition.h"
//
//TypeDefinition::TypeDefinition(Type type, const QString& name)
//    : m_type{ type }
//    , m_name{ name }
//{
//}
//
//void TypeDefinition::addField(Type type, QStringView name, TypedExpression* expression) noexcept
//{
//    m_fields.emplace(name.toString(), new Field(type, name, expression));
//}
//
//Field* TypeDefinition::getFieldByName(QStringView fieldName) const noexcept
//{
//    auto name = fieldName.toString();
//    if (m_fields.contains(name))
//        return m_fields.at(name);
//    else
//        return nullptr;
//}
//
//void TypeDefinition::addFunction(Type type, QStringView name) noexcept
//{
//    m_functions.emplace(type.id(), FunctionDefinition{ type, name.toString() });
//    m_functionNames.emplace(name.toString(), type);
//}
//
//FunctionDefinition& TypeDefinition::getFunctionDefinition(Type type) noexcept
//{
//    static auto invalidFunction = FunctionDefinition{ Type::Undefined(), QString("???") };
//
//    auto id = type.id();
//    if (m_functions.contains(id))
//        return m_functions.at(id);
//    else
//        return invalidFunction;
//}
//
//FunctionDefinition& TypeDefinition::getFunctionDefinitionByName(QStringView fieldName) noexcept
//{
//    auto functionType = Type::Undefined();
//    auto name = fieldName.toString();
//    if (m_functionNames.contains(name))
//        functionType = m_functionNames.at(name);
//
//    return getFunctionDefinition(functionType);
//}
