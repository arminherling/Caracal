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
            const Token& firstColon,
            const Token& secondColon,
            ExpressionUPtr&& rightExpression,
            const Token& semicolon);

        [[nodiscard]] const ExpressionUPtr& leftExpression() const noexcept { return m_leftExpression; }
        [[nodiscard]] const Token& firstColon() const noexcept { return m_firstColon; }
        [[nodiscard]] const Token& secondColon() const noexcept { return m_secondColon; }
        [[nodiscard]] const ExpressionUPtr& rightExpression() const noexcept { return m_rightExpression; }
        [[nodiscard]] const Token& semicolon() const noexcept { return m_semicolon; }

    private:
        ExpressionUPtr m_leftExpression;
        Token m_firstColon;
        Token m_secondColon;
        ExpressionUPtr m_rightExpression;
        Token m_semicolon;
    };
}
