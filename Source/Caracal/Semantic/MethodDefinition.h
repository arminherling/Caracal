#pragma once

#include <Caracal/Semantic/Type.h>
#include <Caracal/Semantic/Parameter.h>
#include <Caracal/Syntax/MethodDefinitionStatement.h>
#include <vector>
#include <string>

namespace Caracal
{
    class MethodDefinition
    {
    public:
        MethodDefinition(
            Type parentType,
            Type type,
            const std::string& name,
            MethodModifier modifier,
            const std::vector<Parameter>& parameters = std::vector<Parameter>(),
            const std::vector<Type>& returnTypes = std::vector<Type>());

        [[nodiscard]] Type parentType() const noexcept { return m_parentType; }
        [[nodiscard]] Type type() const noexcept { return m_type; }
        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        [[nodiscard]] MethodModifier modifier() const noexcept { return m_modifier; }
        [[nodiscard]] const std::vector<Parameter>& parameters() const noexcept { return m_parameters; }
        [[nodiscard]] const std::vector<Type>& returnTypes() const noexcept { return m_returnTypes; }

    private:
        Type m_parentType;
        Type m_type;
        std::string m_name;
        MethodModifier m_modifier;
        std::vector<Parameter> m_parameters;
        std::vector<Type> m_returnTypes;
    };
}
