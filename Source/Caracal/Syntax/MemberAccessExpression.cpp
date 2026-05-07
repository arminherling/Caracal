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

    SourceLocation MemberAccessExpression::sourceLocation(const TokenBuffer& tokens) const
    {
        const auto expressionLocation = m_expression->sourceLocation(tokens);
        const auto dotLocation = tokens.getSourceLocation(m_dot);
        return SourceLocation{ dotLocation.startIndex, expressionLocation.endIndex };
    }
}

