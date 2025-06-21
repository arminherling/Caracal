#include "VariableDeclaration.h"

namespace Caracal
{
    VariableDeclaration::VariableDeclaration(
        ExpressionUPtr&& leftExpression,
        const Token& colonToken,
        std::optional<TypeNameNodeUPtr>&& explicitType,
        const std::optional<Token>& equalToken,
        std::optional<ExpressionUPtr>&& rightExpression,
        const Token& semicolonToken)
        : Statement(NodeKind::VariableDeclaration, (rightExpression.has_value() ? rightExpression.value()->type() : Type::Undefined()))
        , m_leftExpression{ std::move(leftExpression) }
        , m_colonToken{ colonToken }
        , m_explicitType{ std::move(explicitType) }
        , m_equalToken{ equalToken }
        , m_rightExpression{ std::move(rightExpression) }
        , m_semicolonToken{ semicolonToken }
    {
    }
}
