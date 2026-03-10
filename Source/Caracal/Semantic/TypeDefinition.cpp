#include "TypeDefinition.h"

namespace Caracal 
{
    TypeDefinition::TypeDefinition(
        const TypeDefinitionStatement* statement,
        Type type, 
        const std::string& name)
        : m_type{ type }
        , m_name{ name }
        , m_statement{ statement }
    {
    }

    void TypeDefinition::addMethod(Type methodType, const std::string& methodName) noexcept
    {
        m_methods.try_emplace(methodName, methodType);
    }

    Type TypeDefinition::tryGetMethodTypeByName(std::string_view methodName) const noexcept
    {
        if (const auto result = m_methods.find(std::string(methodName)); result != m_methods.end())
            return result->second;

        return Type::Undefined();
    }
}
