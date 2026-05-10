#pragma once

#include <Caracal/API.h>
#include <Caracal/Semantic/AnnotationKind.h>
#include <Caracal/Syntax/ArgumentsNode.h>
#include <Caracal/Syntax/TokenBuffer.h>

namespace Caracal
{
    class CARACAL_API AnnotationNode
    {
    public:
        AnnotationNode(
            AnnotationKind kind,
            const Token& hashToken,
            const Token& nameToken,
            std::string_view name, 
            std::optional<ArgumentsNodeUPtr>&& argumentsNode);

        [[nodiscard]] AnnotationKind kind() const noexcept { return m_kind; }
        [[nodiscard]] const Token& hashToken() const noexcept { return m_hashToken; }
        [[nodiscard]] const Token& nameToken() const noexcept { return m_nameToken; }
        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        [[nodiscard]] const std::optional<ArgumentsNodeUPtr>& argumentsNode() const noexcept { return m_argumentsNode; }
        [[nodiscard]] SourceLocation sourceLocation(const TokenBuffer& tokens) const;
        [[nodiscard]] SourceLocation argumentsLocation(const TokenBuffer& tokens) const;

    private:
        AnnotationKind m_kind;
        Token m_hashToken;
        Token m_nameToken;
        std::string m_name;
        std::optional<ArgumentsNodeUPtr> m_argumentsNode;
    };

    using AnnotationNodeUPtr = std::unique_ptr<AnnotationNode>;
}
