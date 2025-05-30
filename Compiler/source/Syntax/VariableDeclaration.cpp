#include "VariableDeclaration.h"

namespace Caracal
{
    VariableDeclaration::VariableDeclaration(
        const Token& identifier,
        const Token& colon,
        const Token& equal,
        ExpressionUPtr&& expression,
        const Token& semicolon)
        : Statement(NodeKind::VariableDeclaration, expression->type())
        , m_identifier{ identifier }
        , m_colon{ colon }
        , m_equal{ equal }
        , m_expression{ std::move(expression) }
        , m_semicolon{ semicolon }
    {
    }
}
