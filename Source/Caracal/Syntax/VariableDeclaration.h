#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Statement.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/TypeNameNode.h>
#include <Caracal/Syntax/Token.h>

namespace Caracal
{
    class CARACAL_API VariableDeclaration : public Statement
    {
    public:
        VariableDeclaration(
            ExpressionUPtr&& leftExpression,
            const Token& colonToken,
            std::optional<TypeNameNodeUPtr>&& explicitType,
            const std::optional<Token>& equalToken,
            ExpressionUPtr&& rightExpression,
            const Token& semicolonToken);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(VariableDeclaration)

        [[nodiscard]] const ExpressionUPtr& leftExpression() const noexcept { return m_leftExpression; }
        [[nodiscard]] const Token& colonToken() const noexcept { return m_colonToken; }
        [[nodiscard]] const std::optional<TypeNameNodeUPtr>& explicitType() const noexcept { return m_explicitType; }
        [[nodiscard]] const std::optional<Token>& equalToken() const noexcept { return m_equalToken; }
        [[nodiscard]] const ExpressionUPtr& rightExpression() const noexcept { return m_rightExpression; }
        [[nodiscard]] const Token& semicolonToken() const noexcept { return m_semicolonToken; }

    private:
        ExpressionUPtr m_leftExpression;
        Token m_colonToken;
        std::optional<TypeNameNodeUPtr> m_explicitType;
        std::optional<Token> m_equalToken;
        ExpressionUPtr m_rightExpression;
        Token m_semicolonToken;
    };
}
