#pragma once

#include <Compiler/API.h>
#include <Syntax/Statement.h>
#include <Syntax/Expression.h>
#include <Syntax/Token.h>

namespace Caracal
{
    class COMPILER_API VariableDeclaration : public Statement
    {
    public:
        VariableDeclaration(
            const Token& identifier,
            const Token& colon,
            const Token& equal,
            ExpressionUPtr&& expression,
            const Token& semicolon);

        [[nodiscard]] const Token& identifier() const noexcept { return m_identifier; }
        [[nodiscard]] const Token& colon() const noexcept { return m_colon; }
        [[nodiscard]] const Token& equal() const noexcept { return m_equal; }
        [[nodiscard]] const ExpressionUPtr& expression() const noexcept { return m_expression; }
        [[nodiscard]] const Token& semicolon() const noexcept { return m_semicolon; }

    private:
        Token m_identifier;
        Token m_colon;
        Token m_equal;
        ExpressionUPtr m_expression;
        Token m_semicolon;
    };
}
