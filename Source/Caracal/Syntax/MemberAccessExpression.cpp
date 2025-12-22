#include "MemberAccessExpression.h"

namespace Caracal 
{
    MemberAccessExpression::MemberAccessExpression(
        const Token& dot,
        ExpressionUPtr&& expression)
        : Expression(NodeKind::MemberAccessExpression, expression->type())
        , m_dot{ dot }
        , m_expression{ std::move(expression) }
    {
    }
}

