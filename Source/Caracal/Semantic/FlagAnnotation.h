#pragma once

#include <Caracal/Syntax/AnnotationNode.h>

namespace Caracal
{
    class FlagAnnotation : public AnnotationNode
    {
    public:
        FlagAnnotation(
            const Token& hashToken,
            const Token& nameToken,
            std::string_view name,
            std::optional<ArgumentsNodeUPtr>&& argumentsNode);
    };

    using FlagAnnotationUPtr = std::unique_ptr<FlagAnnotation>;
}
