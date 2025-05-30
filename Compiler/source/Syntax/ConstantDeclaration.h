#pragma once

#include <Compiler/API.h>
#include <Syntax/Statement.h>
#include <Syntax/Expression.h>
#include <Syntax/Token.h>

namespace Caracal
{
    class COMPILER_API ConstantDeclaration : public Statement
    {
    public:
        ConstantDeclaration(
            const Token& identifier,
            const Token& firstColon,
            const Token& secondColon,
            ExpressionUPtr&& expression,
            const Token& semicolon);

        [[nodiscard]] const Token& identifier() const noexcept { return m_identifier; }
        [[nodiscard]] const Token& firstColon() const noexcept { return m_firstColon; }
        [[nodiscard]] const Token& secondColon() const noexcept { return m_secondColon; }
        [[nodiscard]] const ExpressionUPtr& expression() const noexcept { return m_expression; }
        [[nodiscard]] const Token& semicolon() const noexcept { return m_semicolon; }

    private:
        Token m_identifier;
        Token m_firstColon;
        Token m_secondColon;
        ExpressionUPtr m_expression;
        Token m_semicolon;
    };
}
