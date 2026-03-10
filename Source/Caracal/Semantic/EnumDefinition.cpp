#include "EnumDefinition.h"

namespace Caracal 
{
    EnumDefinition::EnumDefinition(
        const EnumDefinitionStatement* statement,
        Type type, 
        const std::string& name)
        : m_type{ type }
        , m_baseType{ Type::Undefined() }
        , m_name{ name }
        , m_statement{ statement }
    {
    }
    
    const EnumField& EnumDefinition::getFieldByName(std::string_view fieldName) const noexcept
    {
        return m_fields.at(std::string(fieldName));
    }

    bool EnumDefinition::hasField(std::string_view fieldName) const noexcept
    {
        if(m_fields.contains(std::string(fieldName)))
            return true;
        return false;
    }

    void EnumDefinition::addField(std::string_view name, Expression* expression) noexcept
    {
        m_fields.emplace(std::string(name), EnumField{ name, expression });
    }

    void EnumDefinition::addField(std::string_view name, i32 value) noexcept
    {
        m_fields.emplace(std::string(name), EnumField{ name, value });
    }
}
