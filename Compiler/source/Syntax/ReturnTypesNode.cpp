#include "ReturnTypesNode.h"

namespace Caracal
{
    ReturnTypesNode::ReturnTypesNode(std::vector<ReturnTypeNodeUPtr>&& returnTypes)
        : Node(NodeKind::ReturnTypesNode, Type::Undefined())
        , m_returnTypes{ std::move(returnTypes) }
    {
    }
}
