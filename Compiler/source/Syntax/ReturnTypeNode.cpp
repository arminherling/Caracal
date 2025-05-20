#include "ReturnTypeNode.h"

namespace Caracal
{
    ReturnTypeNode::ReturnTypeNode(const Token& typeName, const Type& type)
        : Node(NodeKind::ReturnTypeNode, type)
        , m_typeName{ typeName }
    {
    }
}
