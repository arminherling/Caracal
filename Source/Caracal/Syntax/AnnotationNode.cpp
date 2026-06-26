#include "AnnotationNode.h"

namespace Caracal
{
    AnnotationNode::AnnotationNode(
        AnnotationKind kind,
        const Token& hashToken,
        const Token& nameToken,
        std::string_view name,
        std::optional<Token> openParenthesisToken,
        std::vector<Argument>&& arguments,
        std::optional<Token> closeParenthesisToken)
        : m_kind{ kind }
        , m_hashToken{ hashToken }
        , m_nameToken{ nameToken }
        , m_name{ name }
        , m_openParenthesisToken{ openParenthesisToken }
        , m_closeParenthesisToken{ closeParenthesisToken }
        , m_arguments{ std::move(arguments) }
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
        if (!m_openParenthesisToken.has_value())
        {
            return sourceLocation(tokens);
        }

        if (m_arguments.empty())
        {
            const auto openParenthesisLocation = tokens.getSourceLocation(m_openParenthesisToken.value());
            const auto closeParenthesisLocation = tokens.getSourceLocation(m_closeParenthesisToken.value());
            return SourceLocation{ openParenthesisLocation.startIndex, closeParenthesisLocation.endIndex };
        }

        const auto firstArgumentLocation = m_arguments.front().value()->sourceLocation(tokens);
        const auto lastArgumentLocation = m_arguments.back().value()->sourceLocation(tokens);
        return SourceLocation{ firstArgumentLocation.startIndex, lastArgumentLocation.endIndex };
    }
}
