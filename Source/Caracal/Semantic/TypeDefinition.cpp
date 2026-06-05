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

    void TypeDefinition::addField(Type fieldType, const std::string& fieldName, i32 fieldIndex, Expression* expression, bool isConstant) noexcept
    {
        m_fields.emplace_back(fieldType, fieldName, fieldIndex, expression, isConstant);
    }

    const FieldDefinition& TypeDefinition::tryGetFieldByName(std::string_view fieldName) const noexcept
    {
        static auto invalidField = FieldDefinition{ Type::Undefined(), std::string("???"), 0, nullptr };

        for (const auto& field : m_fields)
        {
            if (field.name() == fieldName)
                return field;
        }

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
