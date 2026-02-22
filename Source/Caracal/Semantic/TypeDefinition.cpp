#include "TypeDefinition.h"

namespace Caracal 
{
    TypeDefinition::TypeDefinition(Type type, const std::string& name)
        : m_type{ type }
        , m_name{ name }
    {
    }

    void TypeDefinition::addMethod(Type methodType, const std::string& methodName) noexcept
    {
        m_methods.try_emplace(methodName, methodType);
    }
}
