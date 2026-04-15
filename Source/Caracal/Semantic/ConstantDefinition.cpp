#include "ConstantDefinition.h"

namespace Caracal
{
    ConstantDefinition::ConstantDefinition(
        std::string_view name,
        const Expression* expression) noexcept
        : m_name{ name }
        , m_expression{ expression }
    {
    }

    Type ConstantDefinition::type() const noexcept
    {
        return m_expression->type();
    }

    const std::string& ConstantDefinition::name() const noexcept
    {
        return m_name;
    }

    const Expression* ConstantDefinition::expression() const noexcept
    {
        return m_expression;
    }
}
