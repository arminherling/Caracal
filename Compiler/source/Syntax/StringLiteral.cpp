#include "StringLiteral.h"

namespace Caracal
{
    StringLiteral::StringLiteral(const Token& token)
        : Expression(NodeKind::StringLiteral, Type::String())
        , m_token{ token }
    {
    }
}
