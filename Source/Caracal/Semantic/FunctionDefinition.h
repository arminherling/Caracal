#pragma once

#include <Caracal/Defines.h>
#include <Caracal/Semantic/Type.h>
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
            const std::vector<Type>& parameters = std::vector<Type>(),
            const std::vector<Type>& returnTypes = std::vector<Type>());

        [[nodiscard]] Type type() const noexcept { return m_type; }
        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        [[nodiscard]] const std::vector<Type>& parameters() const noexcept { return m_parameters; }
        [[nodiscard]] const std::vector<Type>& returnTypes() const noexcept { return m_returnTypes; }

    private:
        Type m_type;
        std::string m_name;
        std::vector<Type> m_parameters;
        std::vector<Type> m_returnTypes;
    };
}
