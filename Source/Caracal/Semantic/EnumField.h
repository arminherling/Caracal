#pragma once

#include <Caracal/Semantic/Type.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Defines.h>

namespace Caracal
{
    class EnumField
    {
    public:
        EnumField(std::string_view name, Expression* expression);
        EnumField(std::string_view name, i32 value);

        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        [[nodiscard]] Expression* expression() const noexcept { return m_expression; }
        [[nodiscard]] i32 value() const noexcept { return m_value; }

    private:
        std::string m_name;
        Expression* m_expression;
        i32 m_value;
    };
}
