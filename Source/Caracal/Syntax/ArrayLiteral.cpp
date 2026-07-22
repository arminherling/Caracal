#include <Caracal/Syntax/ArrayLiteral.h>

namespace Caracal
{
    ArrayLiteral::ArrayLiteral(
        const Token& openBracketToken,
        std::vector<ExpressionUPtr> elements,
        const std::optional<Token>& ellipsisToken,
        const Token& closeBracketToken)
        : Expression(NodeKind::ArrayLiteral, Type::Undefined())
        , m_openBracketToken{ openBracketToken }
        , m_elements{ std::move(elements) }
        , m_ellipsisToken{ ellipsisToken }
        , m_closeBracketToken{ closeBracketToken }
    {
    }

    SourceLocation ArrayLiteral::sourceLocation(const TokenBuffer& tokens) const
    {
        const auto openLocation = tokens.getSourceLocation(m_openBracketToken);
        const auto closeLocation = tokens.getSourceLocation(m_closeBracketToken);
        return SourceLocation{ openLocation.startIndex, closeLocation.endIndex };
    }
}
