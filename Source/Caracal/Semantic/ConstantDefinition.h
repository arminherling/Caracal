#pragma once

#include <Caracal/API.h>
#include <Caracal/Semantic/Type.h>
#include <Caracal/Syntax/Expression.h>
#include <string>
#include <string_view>

namespace Caracal
{
    class CARACAL_API ConstantDefinition
    {
    public:
        ConstantDefinition(
            std::string_view name,
            const Expression* expression) noexcept;

        [[nodiscard]] Type type() const noexcept;
        [[nodiscard]] const std::string& name() const noexcept;
        [[nodiscard]] const Expression* expression() const noexcept;

    private:
        std::string m_name;
        const Expression* m_expression;
    };
}
