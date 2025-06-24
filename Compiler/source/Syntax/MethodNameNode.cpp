#include <Syntax/MethodNameNode.h>

namespace Caracal 
{
    MethodNameNode::MethodNameNode(
        NameExpressionUPtr&& methodNameExpression)
        : Node(NodeKind::MethodNameNode, Type::Undefined())
        , m_methodNameExpression(std::move(methodNameExpression))
    {
    }

    MethodNameNode::MethodNameNode(
        NameExpressionUPtr&& typeNameExpression, 
        const Token& dotToken, 
        NameExpressionUPtr&& methodNameExpression)
        : Node(NodeKind::MethodNameNode, Type::Undefined())
        , m_typeNameExpression(std::move(typeNameExpression))
        , m_dotToken(dotToken)
        , m_methodNameExpression(std::move(methodNameExpression))
    {
    }
}
