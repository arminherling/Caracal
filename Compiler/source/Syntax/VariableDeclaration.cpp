#include "VariableDeclaration.h"

namespace Caracal
{
    VariableDeclaration::VariableDeclaration(
        ExpressionUPtr&& leftExpression,
        const Token& colonToken,
        const Token& equalToken,
        ExpressionUPtr&& rightExpression,
        const Token& semicolonToken)
        : Statement(NodeKind::VariableDeclaration, rightExpression->type())
        , m_leftExpression{ std::move(leftExpression) }
        , m_colonToken{ colonToken }
        , m_equalToken{ equalToken }
        , m_rightExpression{ std::move(rightExpression) }
        , m_semicolonToken{ semicolonToken }
    {
    }
}
