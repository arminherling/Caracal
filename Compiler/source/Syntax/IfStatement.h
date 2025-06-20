#pragma once

#include <Compiler/API.h>
#include <Syntax/Expression.h>
#include <Syntax/Statement.h>
#include <Syntax/Token.h>

namespace Caracal
{
    class COMPILER_API IfStatement : public Statement
    {
    public:
        IfStatement(
            const Token& ifKeyword,
            ExpressionUPtr&& condition,
            StatementUPtr&& trueStatement);
        IfStatement(
            const Token& ifKeyword,
            ExpressionUPtr&& condition,
            StatementUPtr&& trueStatement,
            const Token& elseKeyword,
            StatementUPtr&& falseStatement);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(IfStatement)

        [[nodiscard]] const Token& ifKeyword() const noexcept { return m_ifKeyword; }
        [[nodiscard]] const ExpressionUPtr& condition() const noexcept { return m_condition; }
        [[nodiscard]] const StatementUPtr& trueStatement() const noexcept { return m_trueStatement; }
        [[nodiscard]] const std::optional<Token>& elseKeyword() const noexcept { return m_elseKeyword; }
        [[nodiscard]] const std::optional<StatementUPtr>& falseStatement() const noexcept { return m_falseStatement; }
        [[nodiscard]] bool hasFalseBlock() const noexcept { return m_falseStatement.has_value(); }

    private:
        Token m_ifKeyword;
        ExpressionUPtr m_condition;
        StatementUPtr m_trueStatement;
        std::optional<Token> m_elseKeyword;
        std::optional<StatementUPtr> m_falseStatement;
    };
}
