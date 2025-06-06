#include "ParameterNode.h"

namespace Caracal
{
    ParameterNode::ParameterNode(
        NameExpressionUPtr&& nameExpression, 
        const Token& colonToken, 
        TypeNameNodeUPtr&& typeName)
        : Node(NodeKind::ParameterNode, typeName->type())
        , m_nameExpression{ std::move(nameExpression) }
        , m_colonToken{ colonToken }
        , m_typeName{ std::move(typeName) }
    {
    }
}
