#include "FunctionDefinition.h"

namespace Caracal
{
    FunctionDefinition::FunctionDefinition(
        const Statement* statement,
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
        , m_statement{ statement }
    {
    }

    void FunctionDefinition::setParameters(const std::vector<Parameter>& parameters) noexcept
    {
        m_parameters = parameters;
    }

    void FunctionDefinition::setReturnTypes(const std::vector<Type>& returnTypes) noexcept
    {
        m_returnTypes = returnTypes;
    }
    
    void FunctionDefinition::setIsVariadic(bool isVariadic) noexcept
    {
        m_isVariadic = isVariadic;
    }

    bool IsBitwiseIntrinsicKind(IntrinsicKind kind) noexcept
    {
        return kind == IntrinsicKind::BitAnd
            || kind == IntrinsicKind::BitOr
            || kind == IntrinsicKind::BitXor
            || kind == IntrinsicKind::BitNot
            || kind == IntrinsicKind::ShiftLeft
            || kind == IntrinsicKind::ShiftRight;
    }
}
