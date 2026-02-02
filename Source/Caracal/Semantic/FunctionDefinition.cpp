#include "FunctionDefinition.h"

namespace Caracal
{
    FunctionDefinition::FunctionDefinition(
        Type type,
        const std::string& name,
        const std::vector<Parameter>& parameters,
        const std::vector<Type>& returnTypes)
        : m_type{ type }
        , m_name{ name }
        , m_parameters{ parameters }
        , m_returnTypes{ returnTypes }
    {
    }
}
