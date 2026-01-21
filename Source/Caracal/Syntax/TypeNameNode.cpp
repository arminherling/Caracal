#include "TypeNameNode.h"

namespace Caracal 
{
    TypeNameNode::TypeNameNode(
        const std::optional<Token>& refToken,
        const Token& nameToken,
        std::string_view name)
        : Node(NodeKind::TypeNameNode, Type::Undefined())
        , m_refToken{ refToken }
        , m_nameToken{ nameToken }
        , m_name{ name }
    {
    }
}
