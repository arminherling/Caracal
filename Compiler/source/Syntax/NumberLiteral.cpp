#include "NumberLiteral.h"

namespace Caracal
{
    NumberLiteral::NumberLiteral(const Token& token)
        : Expression(NodeKind::NumberLiteral, Type::Undefined())
        , m_token{ token }
    {
    }
}
