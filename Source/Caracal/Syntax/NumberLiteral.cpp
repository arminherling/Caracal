#include "NumberLiteral.h"

namespace Caracal
{
    NumberLiteral::NumberLiteral(
        const Token& literalToken,
        std::string_view lexeme,
        const std::optional<Token>& uptickToken,
        std::optional<TypeNameNodeUPtr>&& explicitType)
        : Expression(NodeKind::NumberLiteral, (explicitType.has_value() ? explicitType.value()->type() : Type::Undefined()))
        , m_literalToken{ literalToken }
        , m_lexeme{ std::string(lexeme) }
        , m_uptickToken{ uptickToken }
        , m_explicitType{ std::move(explicitType) }
    {
    }

    SourceLocation NumberLiteral::sourceLocation(const TokenBuffer& tokens) const
    {
        return tokens.getSourceLocation(m_literalToken);
    }
}
