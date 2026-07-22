#include <Caracal/Syntax/ArrayTypeNameNode.h>

namespace Caracal
{
    static std::string BuildDisplayName(
        const TypeNameNodeUPtr& elementType,
        ArrayTypeKind arrayKind,
        const NumberLiteralUPtr& lengthLiteral)
    {
        auto name = std::string{ "[" };
        if (elementType != nullptr)
        {
            name += elementType->name();
        }

        switch (arrayKind)
        {
            case ArrayTypeKind::Slice:
            {
                break;
            }
            case ArrayTypeKind::Fixed:
            {
                name += "; ";
                if (lengthLiteral != nullptr)
                {
                    name += lengthLiteral->literalLexeme();
                }
                break;
            }
            case ArrayTypeKind::Dynamic:
            {
                name += "; _";
                break;
            }
        }

        name += "]";
        return name;
    }

    ArrayTypeNameNode::ArrayTypeNameNode(
        const std::optional<Token>& refToken,
        const Token& openBracketToken,
        TypeNameNodeUPtr elementType,
        ArrayTypeKind arrayKind,
        const std::optional<Token>& semicolonToken,
        NumberLiteralUPtr lengthLiteral,
        const std::optional<Token>& underscoreToken,
        const Token& closeBracketToken)
        : TypeNameNode(NodeKind::ArrayTypeNameNode, refToken, openBracketToken, BuildDisplayName(elementType, arrayKind, lengthLiteral))
        , m_openBracketToken{ openBracketToken }
        , m_elementType{ std::move(elementType) }
        , m_arrayKind{ arrayKind }
        , m_semicolonToken{ semicolonToken }
        , m_lengthLiteral{ std::move(lengthLiteral) }
        , m_underscoreToken{ underscoreToken }
        , m_closeBracketToken{ closeBracketToken }
    {
    }

    SourceLocation ArrayTypeNameNode::sourceLocation(const TokenBuffer& tokens) const
    {
        const auto closeLocation = tokens.getSourceLocation(m_closeBracketToken);
        if (ref().has_value())
        {
            const auto refLocation = tokens.getSourceLocation(ref().value());
            return SourceLocation{ refLocation.startIndex, closeLocation.endIndex };
        }

        const auto openLocation = tokens.getSourceLocation(m_openBracketToken);
        return SourceLocation{ openLocation.startIndex, closeLocation.endIndex };
    }
}
