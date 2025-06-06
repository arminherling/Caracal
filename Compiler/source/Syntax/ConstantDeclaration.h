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
            ExpressionUPtr&& leftExpression,
            const Token& firstColonToken,
            const Token& secondColonToken,
            ExpressionUPtr&& rightExpression,
            const Token& semicolonToken);

        [[nodiscard]] const ExpressionUPtr& leftExpression() const noexcept { return m_leftExpression; }
        [[nodiscard]] const Token& firstColonToken() const noexcept { return m_firstColonToken; }
        [[nodiscard]] const Token& secondColonToken() const noexcept { return m_secondColonToken; }
        [[nodiscard]] const ExpressionUPtr& rightExpression() const noexcept { return m_rightExpression; }
        [[nodiscard]] const Token& semicolonToken() const noexcept { return m_semicolonToken; }

    private:
        ExpressionUPtr m_leftExpression;
        Token m_firstColonToken;
        Token m_secondColonToken;
        ExpressionUPtr m_rightExpression;
        Token m_semicolonToken;
    };
}
