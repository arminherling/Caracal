#include "MethodDefinition.h"

namespace Caracal
{
    MethodDefinition::MethodDefinition(
        Type parentType,
        Type type,
        const std::string& name,
        MethodModifier modifier,
        const std::vector<Parameter>& parameters,
        const std::vector<Type>& returnTypes)
        : m_parentType{ parentType }
        , m_type{ type }
        , m_name{ name }
        , m_modifier{ modifier }
        , m_parameters{ parameters }
        , m_returnTypes{ returnTypes }
    {
    }
}
