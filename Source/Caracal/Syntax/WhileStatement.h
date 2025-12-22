#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/Statement.h>
#include <Caracal/Syntax/Token.h>

namespace Caracal
{
    class CARACAL_API WhileStatement : public Statement
    {
    public:
        WhileStatement(
            const Token& whileKeyword,
            ExpressionUPtr&& condition,
            StatementUPtr&& trueStatement);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(WhileStatement)

        [[nodiscard]] const Token& whileKeyword() const noexcept { return m_whileKeyword; }
        [[nodiscard]] const ExpressionUPtr& condition() const noexcept { return m_condition; }
        [[nodiscard]] const StatementUPtr& trueStatement() const noexcept { return m_trueStatement; }

    private:
        Token m_whileKeyword;
        ExpressionUPtr m_condition;
        StatementUPtr m_trueStatement;
    };
}
