#include "ReturnStatement.h"

namespace Caracal
{
    ReturnStatement::ReturnStatement(
        const Token& returnKeyword,
        std::optional<ExpressionUPtr>&& expression,
        const Token& semicolon)
        : Statement(NodeKind::ReturnStatement, Type::Undefined())
        , m_returnKeyword{ returnKeyword }
        , m_expression{ std::move(expression) }
        , m_semicolon{ semicolon }
    {
    }
}

