#include "NumberLiteral.h"

namespace Caracal
{
    NumberLiteral::NumberLiteral(const Token& literalToken)
        : Expression(NodeKind::NumberLiteral, Type::Undefined())
        , m_literalToken{ literalToken }
    {
    }
}
