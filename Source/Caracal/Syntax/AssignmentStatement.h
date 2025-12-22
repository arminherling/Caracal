#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Statement.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/Token.h>

namespace Caracal
{
    class CARACAL_API AssignmentStatement : public Statement
    {
    public:
        AssignmentStatement(
            ExpressionUPtr&& leftExpression,
            const Token& equalToken,
            ExpressionUPtr&& rightExpression,
            const Token& semicolon);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(AssignmentStatement)

        [[nodiscard]] const ExpressionUPtr& leftExpression() const noexcept { return m_leftExpression; }
        [[nodiscard]] const Token& equalToken() const noexcept { return m_equalToken; }
        [[nodiscard]] const ExpressionUPtr& rightExpression() const noexcept { return m_rightExpression; }
        [[nodiscard]] const Token& semicolonToken() const noexcept { return m_semicolonToken; }

    private:
        ExpressionUPtr m_leftExpression;
        Token m_equalToken;
        ExpressionUPtr m_rightExpression;
        Token m_semicolonToken;
    };
}
