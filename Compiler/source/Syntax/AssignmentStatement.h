#pragma once

#include <Compiler/API.h>
#include <Syntax/Statement.h>
#include <Syntax/Expression.h>
#include <Syntax/Token.h>

namespace Caracal
{
    class COMPILER_API AssignmentStatement : public Statement
    {
    public:
        AssignmentStatement(
            ExpressionUPtr&& leftExpression,
            const Token& equal,
            ExpressionUPtr&& rightExpression,
            const Token& semicolon);

        [[nodiscard]] const ExpressionUPtr& leftExpression() const noexcept { return m_leftExpression; }
        [[nodiscard]] const Token& equal() const noexcept { return m_equal; }
        [[nodiscard]] const ExpressionUPtr& rightExpression() const noexcept { return m_rightExpression; }
        [[nodiscard]] const Token& semicolon() const noexcept { return m_semicolon; }

    private:
        ExpressionUPtr m_leftExpression;
        Token m_equal;
        ExpressionUPtr m_rightExpression;
        Token m_semicolon;
    };
}
