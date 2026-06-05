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
        const auto index = m_fieldIndices.at(std::string(fieldName));
        return m_fields.at(index);
    }

    bool EnumDefinition::hasField(std::string_view fieldName) const noexcept
    {
        if (m_fieldIndices.contains(std::string(fieldName)))
            return true;
        return false;
    }

    void EnumDefinition::addField(std::string_view name, Expression* expression, SourceLocation location) noexcept
    {
        m_fieldIndices.emplace(std::string(name), m_fields.size());
        m_fields.emplace_back(name, expression, location);
    }

    void EnumDefinition::addField(std::string_view name, i32 value, SourceLocation location) noexcept
    {
        m_fieldIndices.emplace(std::string(name), m_fields.size());
        m_fields.emplace_back(name, value, location);
    }
}
