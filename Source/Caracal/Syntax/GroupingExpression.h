#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/Token.h>

namespace Caracal
{
    class CARACAL_API GroupingExpression : public Expression
    {
    public:
        GroupingExpression(
            const Token& openParenthesisToken,
            ExpressionUPtr&& expression,
            const Token& closeParenthesisToken);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(GroupingExpression)

        [[nodiscard]] const Token& openParenthesisToken() const noexcept { return m_openParenthesisToken; }
        [[nodiscard]] const ExpressionUPtr& expression() const noexcept { return m_expression; }
        [[nodiscard]] const Token& closeParenthesisToken() const noexcept { return m_closeParenthesisToken; }

    private:
        Token m_openParenthesisToken;
        ExpressionUPtr m_expression;
        Token m_closeParenthesisToken;
    };
}
