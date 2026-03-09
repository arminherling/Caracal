#pragma once

#include <Caracal/Semantic/Type.h>
#include <optional>
#include <string>

namespace Caracal
{
    class TypeDefinition
    {
    public:
        TypeDefinition(Type type, const std::string& name);

        [[nodiscard]] Type type() const noexcept { return m_type; }
        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        void addMethod(Type methodType, const std::string& methodName) noexcept;
        [[nodiscard]] Type tryGetMethodTypeByName(std::string_view methodName) const noexcept;
        
    private:
        Type m_type;
        std::string m_name;
        std::unordered_map<std::string, Type> m_methods;
    };
}
