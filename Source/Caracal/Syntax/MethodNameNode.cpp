#include "MethodNameNode.h"

namespace Caracal
{
    MethodNameNode::MethodNameNode(
        const Token& methodNameToken,
        std::string_view methodName)
        : Node(NodeKind::MethodNameNode, Type::Undefined())
        , m_methodNameToken(methodNameToken)
        , m_methodName(methodName)
    {
    }

    MethodNameNode::MethodNameNode(
        const Token& typeNameToken,
        std::string_view typeName,
        const Token& dotToken,
        const Token& methodNameToken,
        std::string_view methodName)
        : Node(NodeKind::MethodNameNode, Type::Undefined())
        , m_typeNameToken(typeNameToken)
        , m_typeName(typeName)
        , m_dotToken(dotToken)
        , m_methodNameToken(methodNameToken)
        , m_methodName(methodName)
    {
    }

    std::string Caracal::MethodNameNode::fullMethodName() const noexcept
    {
        if (hasTypeName())
        {
            return typeName().value() + "." + methodName();
        }
        else
        {
            return methodName();
        }
    }
}
