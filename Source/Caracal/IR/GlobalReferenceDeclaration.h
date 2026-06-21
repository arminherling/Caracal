#pragma once

#include <Caracal/Semantic/Type.h>

#include <string>

namespace Caracal
{
    class GlobalReferenceDeclaration
    {
    public:
        GlobalReferenceDeclaration(std::string name, Type type, std::string targetName);

        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        [[nodiscard]] Type type() const noexcept { return m_type; }
        [[nodiscard]] const std::string& targetName() const noexcept { return m_targetName; }

    private:
        std::string m_name;
        Type m_type;
        std::string m_targetName;
    };
}
