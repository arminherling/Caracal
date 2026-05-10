#include "ParametersNode.h"

namespace Caracal
{
    ParametersNode::ParametersNode(
        const Token& openParenthesisToken, 
        std::vector<ParameterNodeUPtr>&& parameters, 
        const Token& closeParenthesisToken)
        : Node(NodeKind::ParametersNode, Type::Undefined())
        , m_openParenthesisToken{ openParenthesisToken }
        , m_parameters{ std::move(parameters) }
        , m_closeParenthesisToken{ closeParenthesisToken }
    {
    }

    SourceLocation ParametersNode::sourceLocation(const TokenBuffer& tokens) const
    {
        const auto openParenthesisLocation = tokens.getSourceLocation(m_openParenthesisToken);
        const auto closeParenthesisLocation = tokens.getSourceLocation(m_closeParenthesisToken);
        return SourceLocation{ openParenthesisLocation.startIndex, closeParenthesisLocation.endIndex };
    }
}
