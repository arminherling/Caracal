#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/Token.h>

#include <optional>
#include <string>

namespace Caracal
{
    class CARACAL_API Argument
    {
    public:
        explicit Argument(ExpressionUPtr&& value);
        Argument(const Token& nameToken, std::string_view name, ExpressionUPtr&& value);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(Argument)

        [[nodiscard]] bool isNamed() const noexcept { return m_nameToken.has_value(); }
        [[nodiscard]] const std::optional<Token>& nameToken() const noexcept { return m_nameToken; }
        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        [[nodiscard]] const ExpressionUPtr& value() const noexcept { return m_value; }

    private:
        std::optional<Token> m_nameToken;
        std::string m_name;
        ExpressionUPtr m_value;
    };
}
