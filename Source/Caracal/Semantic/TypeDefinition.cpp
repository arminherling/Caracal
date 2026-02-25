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

    std::optional<Type> Caracal::TypeDefinition::tryGetMethodTypeByName(std::string_view methodName) const noexcept
    {
        if (const auto result = m_methods.find(std::string(methodName)); result != m_methods.end())
            return result->second;
        return std::nullopt;
    }
}
