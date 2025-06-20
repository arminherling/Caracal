#pragma once

#include <Compiler/API.h>
#include <Syntax/Statement.h>
#include <Syntax/NameExpression.h>
#include <Syntax/Token.h>

namespace Caracal
{
    class COMPILER_API TypeFieldDeclaration : public Statement
    {
    public:
        TypeFieldDeclaration(
            NameExpressionUPtr&& nameExpression,
            const Token& firstColonToken,
            const Token& secondToken,
            ExpressionUPtr&& rightExpression,
            bool isConstant);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(TypeFieldDeclaration)

        [[nodiscard]] const NameExpressionUPtr& nameExpression() const noexcept { return m_nameExpression; }
        [[nodiscard]] const Token& firstColonToken() const noexcept { return m_firstColonToken; }
        [[nodiscard]] const Token& secondToken() const noexcept { return m_secondToken; }
        [[nodiscard]] const ExpressionUPtr& rightExpression() const noexcept { return m_rightExpression; }
        [[nodiscard]] bool isConstant() const noexcept { return m_isConstant; }

    private:
        NameExpressionUPtr m_nameExpression;
        Token m_firstColonToken;
        Token m_secondToken;
        ExpressionUPtr m_rightExpression;
        bool m_isConstant;
    };
}
