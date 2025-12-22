#include "DiscardLiteral.h"

namespace Caracal {
    DiscardLiteral::DiscardLiteral(const Token& underscoreToken)
        : Expression(NodeKind::DiscardLiteral, Type::Discard())
        , m_underscoreToken{ underscoreToken }
    {
    }
}