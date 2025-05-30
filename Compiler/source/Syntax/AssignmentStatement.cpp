#include "AssignmentStatement.h"

namespace Caracal
{
    AssignmentStatement::AssignmentStatement(
        const Token& identifier,
        const Token& equal,
        ExpressionUPtr&& expression,
        const Token& semicolon)
        : Statement(NodeKind::AssignmentStatement, expression->type())
        , m_identifier{ identifier }
        , m_equal{ equal }
        , m_expression{ std::move(expression) }
        , m_semicolon{ semicolon }
    {
    }
}
