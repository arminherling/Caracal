#include "StringLiteral.h"

namespace Caracal
{
    StringLiteral::StringLiteral(const Token& literalToken)
        : Expression(NodeKind::StringLiteral, Type::String())
        , m_literalToken{ literalToken }
    {
    }
}
