#pragma once

#include <Caracal/API.h>
#include <Caracal/Semantic/Type.h>
#include <Caracal/Syntax/Expression.h>
#include <optional>
#include <string>

namespace Caracal
{
    class CARACAL_API FieldDefinition
    {
    public:
        FieldDefinition(Type type, const std::string& name, i32 index, Expression* expression) noexcept;

        [[nodiscard]] Type type() const noexcept { return m_type; }
        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        [[nodiscard]] i32 index() const noexcept { return m_index; }
        [[nodiscard]] Expression* expression() const noexcept { return m_expression; }

    private:
        Type m_type;
        std::string m_name;
        i32 m_index;
        Expression* m_expression;
    };
}
