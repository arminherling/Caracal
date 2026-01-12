#include "StringLiteral.h"

namespace Caracal
{
    StringLiteral::StringLiteral(
        const Token& literalToken,
        const std::string& escapedContent)
        : Expression(NodeKind::StringLiteral, Type::String())
        , m_literalToken{ literalToken }
        , m_escapedContent {escapedContent}
    {
    }
}
