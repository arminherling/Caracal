#pragma once

#include <Compiler/API.h>
#include <Syntax/Expression.h>
#include <Syntax/Token.h>

namespace Caracal 
{
    class COMPILER_API MemberAccessExpression : public Expression
    {
    public:
        MemberAccessExpression(
            const Token& dot, 
            ExpressionUPtr&& expression);

        [[nodiscard]] const Token& dot() const noexcept { return m_dot; }
        [[nodiscard]] const ExpressionUPtr& expression() const noexcept { return m_expression; }

    private:
        Token m_dot;
        ExpressionUPtr m_expression;
    };
}
