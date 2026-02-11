#pragma once

#include <Caracal/Syntax/AnnotationNode.h>

namespace Caracal
{
    class ExternAnnotation : public AnnotationNode
    {
    public:
        ExternAnnotation(
            const Token& hashToken,
            const Token& nameToken,
            std::string_view name,
            std::optional<ArgumentsNodeUPtr>&& argumentsNode);
    };

    using ExternAnnotationUPtr = std::unique_ptr<ExternAnnotation>;
}
