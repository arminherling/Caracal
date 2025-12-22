#include "NumberLiteral.h"

namespace Caracal
{
    NumberLiteral::NumberLiteral(
        const Token& literalToken,
        const std::optional<Token>& uptickToken,
        std::optional<TypeNameNodeUPtr>&& explicitType)
        : Expression(NodeKind::NumberLiteral, (explicitType.has_value() ? explicitType.value()->type() : Type::Undefined()))
        , m_literalToken{ literalToken }
        , m_uptickToken{ uptickToken }
        , m_explicitType{ std::move(explicitType) }
    {
    }
}
