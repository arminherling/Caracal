#include "ParametersNode.h"

namespace Caracal
{
    ParametersNode::ParametersNode(const Token& openParenthesis, std::vector<ParameterNodeUPtr>&& parameters, const Token& closeParenthesis)
        : Node(NodeKind::ParametersNode, Type::Undefined())
        , m_openParenthesis{ openParenthesis }
        , m_parameters{ std::move(parameters) }
        , m_closeParenthesis{ closeParenthesis }
    {
    }
}
