#include "TypeNameNode.h"

namespace Caracal 
{
    TypeNameNode::TypeNameNode(
        const std::optional<Token>& refToken,
        NameExpressionUPtr&& name,
        const Type& type)
        : Node(NodeKind::TypeNameNode, type)
        , m_refToken{ refToken }
        , m_nameExpression{ std::move(name) }
    {
    }
}
