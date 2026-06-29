#pragma once

#include <Caracal/Semantic/Type.h>
#include <vector>
#include <string>

namespace Caracal
{
    class Expression;

    class Parameter
    {
    public:
        Parameter(
            std::string_view name,
            Type type,
            const Expression* defaultValue = nullptr);

        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        [[nodiscard]] Type type() const noexcept { return m_type; }
        [[nodiscard]] bool hasDefault() const noexcept { return m_defaultValue != nullptr; }
        [[nodiscard]] const Expression* defaultValue() const noexcept { return m_defaultValue; }

    private:
        std::string m_name;
        Type m_type;
        const Expression* m_defaultValue;
    };
}
