#pragma once

#include <Caracal/API.h>
#include <Caracal/Semantic/AnnotationKind.h>
#include <Caracal/Syntax/Argument.h>
#include <Caracal/Syntax/Token.h>
#include <Caracal/Syntax/TokenBuffer.h>

#include <optional>
#include <vector>

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
            std::optional<Token> openParenthesisToken,
            std::vector<Argument>&& arguments,
            std::optional<Token> closeParenthesisToken);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(AnnotationNode)

        [[nodiscard]] AnnotationKind kind() const noexcept { return m_kind; }
        [[nodiscard]] const Token& hashToken() const noexcept { return m_hashToken; }
        [[nodiscard]] const Token& nameToken() const noexcept { return m_nameToken; }
        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        [[nodiscard]] bool hasParentheses() const noexcept { return m_openParenthesisToken.has_value(); }
        [[nodiscard]] const std::vector<Argument>& arguments() const noexcept { return m_arguments; }
        [[nodiscard]] SourceLocation sourceLocation(const TokenBuffer& tokens) const;
        [[nodiscard]] SourceLocation argumentsLocation(const TokenBuffer& tokens) const;

    private:
        AnnotationKind m_kind;
        Token m_hashToken;
        Token m_nameToken;
        std::string m_name;
        std::optional<Token> m_openParenthesisToken;
        std::optional<Token> m_closeParenthesisToken;
        std::vector<Argument> m_arguments;
    };

    using AnnotationNodeUPtr = std::unique_ptr<AnnotationNode>;
}
