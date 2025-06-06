#include "ReturnStatement.h"

namespace Caracal
{
    ReturnStatement::ReturnStatement(
        const Token& keywordToken,
        std::optional<ExpressionUPtr>&& expression,
        const Token& semicolonToken)
        : Statement(NodeKind::ReturnStatement, Type::Undefined())
        , m_keywordToken{ keywordToken }
        , m_expression{ std::move(expression) }
        , m_semicolonToken{ semicolonToken }
    {
    }
}

