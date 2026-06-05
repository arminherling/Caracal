#include <Caracal/IR/IRParameter.h>

namespace Caracal
{
    IRParameter::IRParameter(std::string name, Type type) noexcept
        : m_name{ std::move(name) }
        , m_type{ type }
    {
    }
}
