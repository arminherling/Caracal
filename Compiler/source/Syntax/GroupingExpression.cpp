#include "GroupingExpression.h"

namespace Caracal 
{
    GroupingExpression::GroupingExpression(
        const Token& openParenthesis,
        ExpressionUPtr&& expression,
        const Token& closeParenthesis)
        : Expression(NodeKind::GroupingExpression, expression->type())
        , m_openParenthesis{ openParenthesis }
        , m_expression{ std::move(expression)}
        , m_closeParenthesis{ closeParenthesis }
    {
    }
}
