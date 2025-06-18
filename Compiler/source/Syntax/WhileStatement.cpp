#include "WhileStatement.h"

namespace Caracal
{
    WhileStatement::WhileStatement(
        const Token& whileKeyword,
        ExpressionUPtr&& condition,
        StatementUPtr&& trueStatement)
        : Statement{ NodeKind::WhileStatement, Type::Undefined() }
        , m_whileKeyword{ whileKeyword }
        , m_condition{ std::move(condition) }
        , m_trueStatement{ std::move(trueStatement) }
    {
    }
}
