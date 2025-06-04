#include "DiscardLiteral.h"

namespace Caracal {
    DiscardLiteral::DiscardLiteral(const Token& token)
        : Expression(NodeKind::DiscardLiteral, Type::Discard())
    {
    }
}