#include "VariableDeclaration.h"

namespace Caracal
{
    VariableDeclaration::VariableDeclaration(
        ExpressionUPtr&& leftExpression,
        const Token& colon,
        const Token& equal,
        ExpressionUPtr&& rightExpression,
        const Token& semicolon)
        : Statement(NodeKind::VariableDeclaration, rightExpression->type())
        , m_leftExpression{ std::move(leftExpression) }
        , m_colon{ colon }
        , m_equal{ equal }
        , m_rightExpression{ std::move(rightExpression) }
        , m_semicolon{ semicolon }
    {
    }
}
