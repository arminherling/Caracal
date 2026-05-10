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

    SourceLocation ArgumentsNode::sourceLocation(const TokenBuffer& tokens) const
    {
        if (m_arguments.empty())
        {
            const auto openParenthesisLocation = tokens.getSourceLocation(m_openParenthesisToken);
            const auto closeParenthesisLocation = tokens.getSourceLocation(m_closeParenthesisToken);
            return SourceLocation{ openParenthesisLocation.startIndex, closeParenthesisLocation.endIndex };
        }

        const auto firstArgumentLocation = m_arguments.front()->sourceLocation(tokens);
        const auto lastArgumentLocation = m_arguments.back()->sourceLocation(tokens);
        return SourceLocation{ firstArgumentLocation.startIndex, lastArgumentLocation.endIndex };
    }
}
