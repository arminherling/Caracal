#include "BoolLiteral.h"

namespace Caracal
{
    BoolLiteral::BoolLiteral(const Token& literalToken, bool value)
        : Expression(NodeKind::BoolLiteral, Type::Bool())
        , m_literalToken{ literalToken }
        , m_value{ value }
    {
    }
}
