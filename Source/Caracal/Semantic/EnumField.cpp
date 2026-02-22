#include "EnumField.h"

namespace Caracal
{
    EnumField::EnumField(std::string_view name, Expression* expression)
        : m_name{ name }
        , m_expression{ expression }
        , m_value{ 0 }
    {
    }

    EnumField::EnumField(std::string_view name, i32 value)
        : m_name{ name }
        , m_expression{ nullptr }
        , m_value{ value }
    {
    }
}
