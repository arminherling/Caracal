#pragma once

#include <Compiler/API.h>
#include <Syntax/Expression.h>
#include <Syntax/Statement.h>
#include <Syntax/Token.h>

namespace Caracal
{
    class COMPILER_API ExpressionStatement : public Statement
    {
    public:
        ExpressionStatement(
            ExpressionUPtr&& expression,
            const Token& semicolonToken);

        ExpressionStatement(const ExpressionStatement&) = delete;
        ExpressionStatement& operator=(const ExpressionStatement&) = delete;

        ExpressionStatement(ExpressionStatement&&) = default;
        ExpressionStatement& operator=(ExpressionStatement&&) = default;

        [[nodiscard]] const ExpressionUPtr& expression() const noexcept { return m_expression; }
        [[nodiscard]] const Token& semicolonToken() const noexcept { return m_semicolonToken; }

    private:
        ExpressionUPtr m_expression;
        Token m_semicolonToken;
    };
}
