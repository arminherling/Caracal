#include "ErrorExpression.h"

namespace Caracal
{
    ErrorExpression::ErrorExpression(const Token& errorToken)
        : Expression(NodeKind::Error, Type::Undefined())
        , m_errorToken{ errorToken }
    {
    }

    SourceLocation ErrorExpression::sourceLocation(const TokenBuffer& tokens) const
    {
        return tokens.getSourceLocation(m_errorToken);
    }
}
