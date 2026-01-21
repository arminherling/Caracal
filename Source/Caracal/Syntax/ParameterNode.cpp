#include "ParameterNode.h"

namespace Caracal
{
    ParameterNode::ParameterNode(
        const Token& nameToken,
        std::string_view name,
        const Token& colonToken, 
        TypeNameNodeUPtr&& typeName)
        : Node(NodeKind::ParameterNode, typeName->type())
        , m_nameToken{ nameToken }
        , m_name{ name }
        , m_colonToken{ colonToken }
        , m_typeName{ std::move(typeName) }
    {
    }
}
