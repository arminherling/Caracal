#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Statement.h>
#include <Caracal/Syntax/Token.h>
#include <Caracal/Syntax/Expression.h>

namespace Caracal
{
    class CARACAL_API ReturnStatement : public Statement
    {
    public:
        ReturnStatement(
            const Token& keywordToken,
            std::optional<ExpressionUPtr>&& expression,
            const Token& semicolonToken);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(ReturnStatement)

        [[nodiscard]] const Token& keywordToken() const noexcept { return m_keywordToken; }
        [[nodiscard]] const std::optional<ExpressionUPtr>& expression() const noexcept { return m_expression; }
        [[nodiscard]] const Token& semicolonToken() const noexcept { return m_semicolonToken; }

    private:
        Token m_keywordToken;
        std::optional<ExpressionUPtr> m_expression;
        Token m_semicolonToken;
    };
}
