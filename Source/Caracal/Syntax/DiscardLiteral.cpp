#include "DiscardLiteral.h"

namespace Caracal {
    DiscardLiteral::DiscardLiteral(const Token& underscoreToken)
        : Expression(NodeKind::DiscardLiteral, Type::Discard())
        , m_underscoreToken{ underscoreToken }
    {
    }

    SourceLocation DiscardLiteral::sourceLocation(const TokenBuffer& tokens) const
    {
        return tokens.getSourceLocation(m_underscoreToken);
    }
}