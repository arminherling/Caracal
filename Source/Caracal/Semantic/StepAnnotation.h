#pragma once

#include <Caracal/Syntax/AnnotationNode.h>

namespace Caracal
{
    class StepAnnotation : public AnnotationNode
    {
    public:
        StepAnnotation(
            const Token& hashToken,
            const Token& nameToken,
            std::string_view name,
            std::optional<ArgumentsNodeUPtr>&& argumentsNode);
    };

    using StepAnnotationUPtr = std::unique_ptr<StepAnnotation>;
}
