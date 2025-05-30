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
            const Token& identifier,
            const Token& equal,
            ExpressionUPtr&& expression,
            const Token& semicolon);

        [[nodiscard]] const Token& identifier() const noexcept { return m_identifier; }
        [[nodiscard]] const Token& equal() const noexcept { return m_equal; }
        [[nodiscard]] const ExpressionUPtr& expression() const noexcept { return m_expression; }
        [[nodiscard]] const Token& semicolon() const noexcept { return m_semicolon; }

    private:
        Token m_identifier;
        Token m_equal;
        ExpressionUPtr m_expression;
        Token m_semicolon;
    };
}
