#include "AssignmentStatement.h"

namespace Caracal
{
    AssignmentStatement::AssignmentStatement(
        ExpressionUPtr&& leftExpression,
        const Token& equal,
        ExpressionUPtr&& rightExpression,
        const Token& semicolon)
        : Statement(NodeKind::AssignmentStatement, rightExpression->type())
        , m_leftExpression{ std::move(leftExpression) }
        , m_equal{ equal }
        , m_rightExpression{ std::move(rightExpression) }
        , m_semicolon{ semicolon }
    {
    }
}
