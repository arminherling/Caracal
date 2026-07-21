#include "NumberLiteral.h"

namespace Caracal
{
    NumberLiteral::NumberLiteral(
        const Token& literalToken,
        std::string_view lexeme,
        const std::optional<Token>& singleQuoteToken,
        std::optional<TypeNameNodeUPtr>&& explicitType)
        : Expression(NodeKind::NumberLiteral, (explicitType.has_value() ? explicitType.value()->type() : Type::Undefined()))
        , m_literalToken{ literalToken }
        , m_lexeme{ std::string(lexeme) }
        , m_singleQuoteToken{ singleQuoteToken }
        , m_explicitType{ std::move(explicitType) }
    {
    }

    SourceLocation NumberLiteral::sourceLocation(const TokenBuffer& tokens) const
    {
        return tokens.getSourceLocation(m_literalToken);
    }

    void NumberLiteral::setParsedValue(std::optional<ParsedValue> value) noexcept
    {
        m_parsedValue = std::move(value);
    }
}
