#include "TypeNameNode.h"

namespace Caracal 
{
    TypeNameNode::TypeNameNode(
        const std::optional<Token>& refToken,
        NameExpressionUPtr&& name)
        : Node(NodeKind::TypeNameNode, Type::Undefined())
        , m_refToken{ refToken }
        , m_nameExpression{ std::move(name) }
    {
    }
}
