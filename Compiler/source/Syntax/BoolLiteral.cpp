#include "BoolLiteral.h"

namespace Caracal
{
    BoolLiteral::BoolLiteral(const Token& token, bool value)
        : Expression(NodeKind::BoolLiteral, Type::Bool())
        , m_token{ token }
        , m_value{ value }
    {
    }
}
