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
}
