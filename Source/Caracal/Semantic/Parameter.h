#pragma once

#include <Caracal/Semantic/Type.h>
#include <vector>
#include <string>

namespace Caracal
{
    class Parameter
    {
    public:
        Parameter(
            std::string_view name,
            Type type);

        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        [[nodiscard]] Type type() const noexcept { return m_type; }

    private:
        std::string m_name;
        Type m_type;
    };
}
