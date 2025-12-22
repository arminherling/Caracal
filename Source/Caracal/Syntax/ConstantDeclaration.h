#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Statement.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/TypeNameNode.h>
#include <Caracal/Syntax/Token.h>

namespace Caracal
{
    class CARACAL_API ConstantDeclaration : public Statement
    {
    public:
        ConstantDeclaration(
            ExpressionUPtr&& leftExpression,
            const Token& firstColonToken,
            std::optional<TypeNameNodeUPtr>&& explicitType,
            const Token& secondColonToken,
            ExpressionUPtr&& rightExpression,
            const Token& semicolonToken);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(ConstantDeclaration)

        [[nodiscard]] const ExpressionUPtr& leftExpression() const noexcept { return m_leftExpression; }
        [[nodiscard]] const Token& firstColonToken() const noexcept { return m_firstColonToken; }
        [[nodiscard]] const std::optional<TypeNameNodeUPtr>& explicitType() const noexcept { return m_explicitType; }
        [[nodiscard]] const Token& secondColonToken() const noexcept { return m_secondColonToken; }
        [[nodiscard]] const ExpressionUPtr& rightExpression() const noexcept { return m_rightExpression; }
        [[nodiscard]] const Token& semicolonToken() const noexcept { return m_semicolonToken; }

    private:
        ExpressionUPtr m_leftExpression;
        Token m_firstColonToken;
        std::optional<TypeNameNodeUPtr> m_explicitType;
        Token m_secondColonToken;
        ExpressionUPtr m_rightExpression;
        Token m_semicolonToken;
    };
}
