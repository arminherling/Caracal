#include "StepAnnotation.h"

namespace Caracal
{
    StepAnnotation::StepAnnotation(
        const Token& hashToken,
        const Token& nameToken,
        std::string_view name,
        std::optional<ArgumentsNodeUPtr>&& argumentsNode)
        : AnnotationNode(AnnotationKind::Step, hashToken, nameToken, name, std::move(argumentsNode))
    {
    }
}
