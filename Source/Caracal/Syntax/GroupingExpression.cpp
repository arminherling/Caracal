#include "GroupingExpression.h"

namespace Caracal 
{
    GroupingExpression::GroupingExpression(
        const Token& openParenthesisToken,
        ExpressionUPtr&& expression,
        const Token& closeParenthesisToken)
        : Expression(NodeKind::GroupingExpression, expression->type())
        , m_openParenthesisToken{ openParenthesisToken }
        , m_expression{ std::move(expression)}
        , m_closeParenthesisToken{ closeParenthesisToken }
    {
    }
}
