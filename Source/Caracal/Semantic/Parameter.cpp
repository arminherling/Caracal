#include "Parameter.h"

namespace Caracal
{
    Parameter::Parameter(
        std::string_view name,
        Type type)
        : m_name{ name }
        , m_type{ type }
    {
    }
}
