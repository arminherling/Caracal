#include <Caracal/IR/GlobalReferenceDeclaration.h>

#include <utility>

namespace Caracal
{
    GlobalReferenceDeclaration::GlobalReferenceDeclaration(std::string name, Type type, std::string targetName)
        : m_name{ std::move(name) }
        , m_type{ type }
        , m_targetName{ std::move(targetName) }
    {
    }
}
