#include "ParameterNode.h"

namespace Caracal
{
    ParameterNode::ParameterNode(
        const Token& nameToken,
        std::string_view name,
        const Token& colonToken,
        TypeNameNodeUPtr&& typeName,
        bool isVariadic,
        ExpressionUPtr defaultValue)
        : Node(NodeKind::ParameterNode, typeName->type())
        , m_nameToken{ nameToken }
        , m_name{ name }
        , m_colonToken{ colonToken }
        , m_typeName{ std::move(typeName) }
        , m_isVariadic{ isVariadic }
        , m_defaultValue{ std::move(defaultValue) }
    {
    }

    SourceLocation ParameterNode::sourceLocation(const TokenBuffer& tokens) const
    {
        const auto startLocation = tokens.getSourceLocation(m_nameToken);
        const auto endLocation = tokens.getSourceLocation(m_typeName->nameToken());
        return SourceLocation{ startLocation.startIndex, endLocation.endIndex };
    }
}
