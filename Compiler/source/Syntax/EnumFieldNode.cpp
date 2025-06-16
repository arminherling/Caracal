#include "EnumFieldNode.h"

namespace Caracal
{
    EnumFieldNode::EnumFieldNode(
        NameExpressionUPtr&& nameExpression)
        : Node(NodeKind::EnumFieldNode, Type::Undefined())
        , m_nameExpression{ std::move(nameExpression) }
        , m_colon1{ std::nullopt }
        , m_colon2{ std::nullopt }
        , m_valueExpression{ std::nullopt }
    {
    }

    EnumFieldNode::EnumFieldNode(
        NameExpressionUPtr&& nameExpression, 
        const Token& colon1, 
        const Token& colon2, 
        ExpressionUPtr&& valueExpression)
        : Node(NodeKind::EnumFieldNode, Type::Undefined())
        , m_nameExpression{ std::move(nameExpression) }
        , m_colon1{ colon1 }
        , m_colon2{ colon2 }
        , m_valueExpression{ std::move(valueExpression) }
    {
    }
}
