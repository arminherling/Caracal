#pragma once

#include <Caracal/Semantic/Type.h>

#include <string>
#include <utility>
#include <vector>

namespace Caracal
{
    class ExternFunction
    {
    public:
        ExternFunction() = default;
        ExternFunction(std::string name, const std::vector<Type>& parameterTypes, Type returnType);

        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        [[nodiscard]] const std::vector<Type>& parameterTypes() const noexcept { return m_parameterTypes; }
        [[nodiscard]] Type returnType() const noexcept { return m_returnType; }

        void addParameterType(Type type);

    private:
        std::string m_name;
        std::vector<Type> m_parameterTypes;
        Type m_returnType{ Type::Void() };
    };
}
