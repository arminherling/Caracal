#pragma once

#include <Compiler/API.h>
#include <Syntax/Expression.h>
#include <Syntax/Token.h>

namespace Caracal
{
    class COMPILER_API GroupingExpression : public Expression
    {
    public:
        GroupingExpression(
            const Token& openParenthesisToken,
            ExpressionUPtr&& expression,
            const Token& closeParenthesisToken);

        [[nodiscard]] const Token& openParenthesisToken() const noexcept { return m_openParenthesisToken; }
        [[nodiscard]] const ExpressionUPtr& expression() const noexcept { return m_expression; }
        [[nodiscard]] const Token& closeParenthesisToken() const noexcept { return m_closeParenthesisToken; }

    private:
        Token m_openParenthesisToken;
        ExpressionUPtr m_expression;
        Token m_closeParenthesisToken;
    };
}
