#include "AssignmentStatement.h"

namespace Caracal
{
    AssignmentStatement::AssignmentStatement(
        ExpressionUPtr&& leftExpression,
        const Token& equalToken,
        ExpressionUPtr&& rightExpression,
        const Token& semicolonToken)
        : Statement(NodeKind::AssignmentStatement, rightExpression->type())
        , m_leftExpression{ std::move(leftExpression) }
        , m_equalToken{ equalToken }
        , m_rightExpression{ std::move(rightExpression) }
        , m_semicolonToken{ semicolonToken }
    {
    }
}
