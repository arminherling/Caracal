#pragma once

#include <Caracal/IR/ConstantValue.h>
#include <Caracal/Semantic/Type.h>

#include <string>

namespace Caracal
{
    class GlobalConstantDeclaration
    {
    public:
        GlobalConstantDeclaration(std::string name, Type type, ConstantValue value);

        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        [[nodiscard]] Type type() const noexcept { return m_type; }
        [[nodiscard]] const ConstantValue& value() const noexcept { return m_value; }

    private:
        std::string m_name;
        Type m_type;
        ConstantValue m_value;
    };
}
