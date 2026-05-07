#include "BoolLiteral.h"

namespace Caracal
{
    BoolLiteral::BoolLiteral(const Token& literalToken, bool value)
        : Expression(NodeKind::BoolLiteral, Type::Bool())
        , m_literalToken{ literalToken }
        , m_value{ value }
    {
    }

    SourceLocation BoolLiteral::sourceLocation(const TokenBuffer& tokens) const
    {
        return tokens.getSourceLocation(m_literalToken);
    }
}
