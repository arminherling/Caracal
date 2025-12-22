#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/Statement.h>
#include <Caracal/Syntax/Token.h>

namespace Caracal
{
    class CARACAL_API ExpressionStatement : public Statement
    {
    public:
        ExpressionStatement(
            ExpressionUPtr&& expression,
            const Token& semicolonToken);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(ExpressionStatement)

        [[nodiscard]] const ExpressionUPtr& expression() const noexcept { return m_expression; }
        [[nodiscard]] const Token& semicolonToken() const noexcept { return m_semicolonToken; }

    private:
        ExpressionUPtr m_expression;
        Token m_semicolonToken;
    };
}
