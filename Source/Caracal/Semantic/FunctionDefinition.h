#pragma once

#include <Caracal/Semantic/Type.h>
#include <Caracal/Semantic/Parameter.h>
#include <vector>
#include <string>

namespace Caracal
{
    class FunctionDefinition
    {
    public:
        FunctionDefinition(
            Type type,
            const std::string& name,
            bool isVariadic,
            const std::vector<Parameter>& parameters = std::vector<Parameter>(),
            const std::vector<Type>& returnTypes = std::vector<Type>());

        [[nodiscard]] Type type() const noexcept { return m_type; }
        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        [[nodiscard]] bool isVariadic() const noexcept { return m_isVariadic; }
        [[nodiscard]] const std::vector<Parameter>& parameters() const noexcept { return m_parameters; }
        [[nodiscard]] const std::vector<Type>& returnTypes() const noexcept { return m_returnTypes; }

    private:
        Type m_type;
        std::string m_name;
        bool m_isVariadic;
        std::vector<Parameter> m_parameters;
        std::vector<Type> m_returnTypes;
    };
}
