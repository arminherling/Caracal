#include "TypeNameNode.h"

namespace Caracal 
{
    TypeNameNode::TypeNameNode(
        const std::optional<Token>& refToken,
        const Token& nameToken,
        std::string_view name)
        : TypeNameNode(NodeKind::TypeNameNode, refToken, nameToken, name)
    {
    }

    TypeNameNode::TypeNameNode(
        NodeKind kind,
        const std::optional<Token>& refToken,
        const Token& nameToken,
        std::string_view name)
        : Node(kind, Type::Undefined())
        , m_refToken{ refToken }
        , m_nameToken{ nameToken }
        , m_name{ name }
    {
    }

    SourceLocation TypeNameNode::sourceLocation(const TokenBuffer& tokens) const
    {
        const auto nameLocation = tokens.getSourceLocation(m_nameToken);
        if (!m_refToken.has_value())
        {
            return nameLocation;
        }

        const auto refLocation = tokens.getSourceLocation(m_refToken.value());
        return SourceLocation{ refLocation.startIndex, nameLocation.endIndex };
    }
}
