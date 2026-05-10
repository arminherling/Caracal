#include "AnnotationNode.h"

namespace Caracal 
{
    AnnotationNode::AnnotationNode(
        AnnotationKind kind,
        const Token& hashToken,
        const Token& nameToken,
        std::string_view name,
        std::optional<ArgumentsNodeUPtr>&& argumentsNode)
        : m_kind{ kind }
        , m_hashToken{ hashToken }
        , m_nameToken{ nameToken }
        , m_name{ name }
        , m_argumentsNode{ std::move(argumentsNode) }
    {
    }

    SourceLocation AnnotationNode::sourceLocation(const TokenBuffer& tokens) const
    {
        const auto hashLocation = tokens.getSourceLocation(m_hashToken);
        const auto nameLocation = tokens.getSourceLocation(m_nameToken);
        return SourceLocation{ hashLocation.startIndex, nameLocation.endIndex };
    }

    SourceLocation AnnotationNode::argumentsLocation(const TokenBuffer& tokens) const
    {
        if (!m_argumentsNode.has_value())
        {
            return sourceLocation(tokens);
        }

        return m_argumentsNode.value()->sourceLocation(tokens);
    }
}
