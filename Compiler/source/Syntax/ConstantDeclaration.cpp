#include "ConstantDeclaration.h"

namespace Caracal
{
    ConstantDeclaration::ConstantDeclaration(
        ExpressionUPtr&& leftExpression,
        const Token& firstColonToken,
        const Token& secondColonToken,
        ExpressionUPtr&& rightExpression,
        const Token& semicolonToken)
        : Statement(NodeKind::ConstantDeclaration, rightExpression->type())
        , m_leftExpression{ std::move(leftExpression) }
        , m_firstColonToken{ firstColonToken }
        , m_secondColonToken{ secondColonToken }
        , m_rightExpression{ std::move(rightExpression) }
        , m_semicolonToken{ semicolonToken }
    {
    }
}
