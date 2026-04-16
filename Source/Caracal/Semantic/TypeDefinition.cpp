#include "TypeDefinition.h"

namespace Caracal 
{
    TypeDefinition::TypeDefinition(
        const TypeDefinitionStatement* statement,
        Type type, 
        const std::string& name)
        : m_type{ type }
        , m_name{ name }
        , m_statement{ statement }
    {
    }

    void TypeDefinition::addField(Type fieldType, const std::string& fieldName, Expression* expression) noexcept
    {
        m_fields.try_emplace(fieldName, fieldType, fieldName, expression);
    }

    const FieldDefinition& TypeDefinition::tryGetFieldByName(std::string_view fieldName) const noexcept
    {
        static auto invalidField = FieldDefinition{ Type::Undefined(), std::string("???"), nullptr };

        if (const auto result = m_fields.find(std::string(fieldName)); result != m_fields.end())
            return result->second;

        return invalidField;
    }

    void TypeDefinition::addMethod(Type methodType, const std::string& methodName) noexcept
    {
        m_methods.try_emplace(methodName, methodType);
    }

    Type TypeDefinition::tryGetMethodTypeByName(std::string_view methodName) const noexcept
    {
        if (const auto result = m_methods.find(std::string(methodName)); result != m_methods.end())
            return result->second;

        return Type::Undefined();
    }
}
