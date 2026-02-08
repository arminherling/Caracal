#include "AnnotationNode.h"

namespace Caracal 
{
    AnnotationNode::AnnotationNode(
        const Token& hashToken,
        const Token& nameToken,
        std::string_view name,
        ArgumentsNodeUPtr&& argumentsNode)
        : Node(NodeKind::AnnotationNode, Type::Undefined())
        , m_hashToken{ hashToken }
        , m_nameToken{ nameToken }
        , m_name{ name }
        , m_argumentsNode{ std::move(argumentsNode) }
    {
    }
}
