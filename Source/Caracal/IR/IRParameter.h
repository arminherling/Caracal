#pragma once

#include <Caracal/Semantic/Type.h>

#include <string>

namespace Caracal
{
    class IRParameter
    {
    public:
        IRParameter(std::string name, Type type) noexcept;

        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        [[nodiscard]] Type type() const noexcept { return m_type; }

    private:
        std::string m_name;
        Type m_type;
    };
}
