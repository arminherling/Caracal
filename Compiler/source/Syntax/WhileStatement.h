#pragma once

#include <Compiler/API.h>
#include <Syntax/Expression.h>
#include <Syntax/Statement.h>
#include <Syntax/Token.h>

namespace Caracal
{
    class COMPILER_API WhileStatement : public Statement
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
