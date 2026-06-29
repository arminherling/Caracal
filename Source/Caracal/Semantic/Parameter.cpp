#include "Parameter.h"

namespace Caracal
{
    Parameter::Parameter(
        std::string_view name,
        Type type,
        const Expression* defaultValue)
        : m_name{ name }
        , m_type{ type }
        , m_defaultValue{ defaultValue }
    {
    }
}
