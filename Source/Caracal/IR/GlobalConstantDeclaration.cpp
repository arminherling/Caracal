#include <Caracal/IR/GlobalConstantDeclaration.h>

#include <utility>

namespace Caracal
{
    GlobalConstantDeclaration::GlobalConstantDeclaration(std::string name, Type type, ConstantValue value)
        : m_name{ std::move(name) }
        , m_type{ type }
        , m_value{ std::move(value) }
    {
    }
}
