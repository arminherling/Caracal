#include "ExternAnnotation.h"

namespace Caracal
{
    ExternAnnotation::ExternAnnotation(
        const Token& hashToken, 
        const Token& nameToken, 
        std::string_view name, 
        std::optional<ArgumentsNodeUPtr>&& argumentsNode)
        : AnnotationNode(AnnotationKind::Extern, hashToken, nameToken, name, std::move(argumentsNode))
    {
    }
}
