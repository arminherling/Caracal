#pragma once

#include <Caracal/Semantic/Type.h>

#include <string>

namespace Caracal
{
    // a global that is initialized from constructors or functions
    // the storage is zero-initialized constructed in the global initializer function
    class ConstructedGlobalDeclaration
    {
    public:
        ConstructedGlobalDeclaration(std::string name, Type type);

        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        [[nodiscard]] Type type() const noexcept { return m_type; }

    private:
        std::string m_name;
        Type m_type;
    };
}
