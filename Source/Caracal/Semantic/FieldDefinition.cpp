#include "FieldDefinition.h"

namespace Caracal
{
    FieldDefinition::FieldDefinition(Type type, const std::string& name, Expression* expression) noexcept
        : m_type{ type }
        , m_name{ name }
        , m_expression{ expression }
    {
    }
}
