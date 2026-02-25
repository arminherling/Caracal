#include "FunctionDefinition.h"

namespace Caracal
{
    FunctionDefinition::FunctionDefinition(
        Type type,
        Type parentType,
        FunctionType functionType,
        const std::string& name,
        const std::string& fullName,
        bool isVariadic,
        const std::vector<Parameter>& parameters,
        const std::vector<Type>& returnTypes)
        : m_type{ type }
        , m_parentType{ parentType }
        , m_functionType{ functionType }
        , m_name{ name }
        , m_fullName{ fullName }
        , m_isVariadic{ isVariadic }
        , m_parameters{ parameters }
        , m_returnTypes{ returnTypes }
    {
    }
}
