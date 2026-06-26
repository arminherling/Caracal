#include "Argument.h"

namespace Caracal
{
    Argument::Argument(ExpressionUPtr&& value)
        : m_value{ std::move(value) }
    {
    }

    Argument::Argument(const Token& nameToken, std::string_view name, ExpressionUPtr&& value)
        : m_nameToken{ nameToken }
        , m_name{ name }
        , m_value{ std::move(value) }
    {
    }
}
