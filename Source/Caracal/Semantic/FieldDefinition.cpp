#include "FieldDefinition.h"

namespace Caracal
{
    FieldDefinition::FieldDefinition(Type type, const std::string& name, i32 index, Expression* expression, bool isConstant) noexcept
        : m_type{ type }
        , m_name{ name }
        , m_index{ index }
        , m_expression{ expression }
        , m_isConstant{ isConstant }
    {
    }
}
