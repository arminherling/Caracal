#include "FieldDefinition.h"

namespace Caracal
{
    FieldDefinition::FieldDefinition(Type type, const std::string& name, i32 index, Expression* expression) noexcept
        : m_type{ type }
        , m_name{ name }
        , m_index{ index }
        , m_expression{ expression }
    {
    }
}
