#pragma once

#include <Caracal/Semantic/Type.h>
#include <Caracal/Semantic/EnumField.h>

namespace Caracal
{
    class EnumDefinition
    {
    public:
        EnumDefinition(Type type, const std::string& name);

        [[nodiscard]] Type type() const noexcept { return m_type; }
        [[nodiscard]] Type baseType() const noexcept { return m_baseType; }
        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        //[[nodiscard]] EnumField& getFieldByName(std::string_view fieldName) const noexcept;
        [[nodiscard]] bool hasField(std::string_view fieldName) const noexcept;

        void setBaseType(Type baseType) noexcept { m_baseType = baseType; }
        void addField(std::string_view name, Expression* expression) noexcept;
        void addField(std::string_view name, i32 value) noexcept;

    private:
        Type m_type;
        Type m_baseType;
        std::string m_name;
        std::unordered_map<std::string, EnumField> m_fields;
    };
}
