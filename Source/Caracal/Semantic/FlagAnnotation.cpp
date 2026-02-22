#include "FlagAnnotation.h"

namespace Caracal
{
    FlagAnnotation::FlagAnnotation(
        const Token& hashToken, 
        const Token& nameToken, 
        std::string_view name, 
        std::optional<ArgumentsNodeUPtr>&& argumentsNode)
        : AnnotationNode(AnnotationKind::Flag, hashToken, nameToken, name, std::move(argumentsNode))
    {
    }
}
