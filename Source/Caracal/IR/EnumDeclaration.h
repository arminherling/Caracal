#pragma once

#include <Caracal/IR/ConstantValue.h>
#include <Caracal/Semantic/Type.h>

#include <string>
#include <utility>
#include <vector>

namespace Caracal
{
    class EnumDeclaration
    {
    public:
        struct Field final
        {
            std::string name;
            ConstantValue value;
        };

        EnumDeclaration() = default;
        EnumDeclaration(std::string name, Type type, Type baseType);

        [[nodiscard]] Type type() const noexcept { return m_type; }
        [[nodiscard]] Type baseType() const noexcept { return m_baseType; }
        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        [[nodiscard]] const std::vector<Field>& fields() const noexcept { return m_fields; }

        void addField(std::string name, ConstantValue value);

    private:
        Type m_type{ Type::Undefined() };
        Type m_baseType{ Type::Undefined() };
        std::string m_name;
        std::vector<Field> m_fields;
    };
}
