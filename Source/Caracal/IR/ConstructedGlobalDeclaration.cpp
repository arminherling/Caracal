#include <Caracal/IR/ConstructedGlobalDeclaration.h>

#include <utility>

namespace Caracal
{
    ConstructedGlobalDeclaration::ConstructedGlobalDeclaration(std::string name, Type type)
        : m_name{ std::move(name) }
        , m_type{ type }
    {
    }
}
