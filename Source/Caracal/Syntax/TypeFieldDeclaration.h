#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Statement.h>
#include <Caracal/Syntax/NameExpression.h>
#include <Caracal/Syntax/TypeNameNode.h>
#include <Caracal/Syntax/Token.h>

namespace Caracal
{
    class CARACAL_API TypeFieldDeclaration : public Statement
    {
    public:
        TypeFieldDeclaration(
            NameExpressionUPtr&& nameExpression,
            const Token& firstColonToken,
            std::optional<TypeNameNodeUPtr>&& explicitType,
            const std::optional<Token>& secondToken,
            std::optional<ExpressionUPtr>&& rightExpression,
            bool isConstant);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(TypeFieldDeclaration)

        [[nodiscard]] const NameExpressionUPtr& nameExpression() const noexcept { return m_nameExpression; }
        [[nodiscard]] const Token& firstColonToken() const noexcept { return m_firstColonToken; }
        [[nodiscard]] const std::optional<TypeNameNodeUPtr>& explicitType() const noexcept { return m_explicitType; }
        [[nodiscard]] const std::optional<Token>& secondToken() const noexcept { return m_secondToken; }
        [[nodiscard]] const std::optional<ExpressionUPtr>& rightExpression() const noexcept { return m_rightExpression; }
        [[nodiscard]] bool isConstant() const noexcept { return m_isConstant; }

    private:
        NameExpressionUPtr m_nameExpression;
        Token m_firstColonToken;
        std::optional<TypeNameNodeUPtr> m_explicitType;
        std::optional<Token> m_secondToken;
        std::optional<ExpressionUPtr> m_rightExpression;
        bool m_isConstant;
    };
}
