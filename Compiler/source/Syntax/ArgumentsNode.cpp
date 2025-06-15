#include "ArgumentsNode.h"

namespace Caracal
{
    ArgumentsNode::ArgumentsNode(
        const Token& openParenthesisToken,
        std::vector<ExpressionUPtr>&& arguments,
        const Token& closeParenthesisToken)
        : Node(NodeKind::ArgumentsNode, Type::Undefined())
        , m_openParenthesisToken{ openParenthesisToken }
        , m_arguments{ std::move(arguments) }
        , m_closeParenthesisToken{ closeParenthesisToken }
    {
    }
}
