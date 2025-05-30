#include "ConstantDeclaration.h"

namespace Caracal
{
    ConstantDeclaration::ConstantDeclaration(
        const Token& identifier,
        const Token& firstColon,
        const Token& secondColon,
        ExpressionUPtr&& expression,
        const Token& semicolon)
        : Statement(NodeKind::ConstantDeclaration, expression->type())
        , m_identifier{ identifier }
        , m_firstColon{ firstColon }
        , m_secondColon{ secondColon }
        , m_expression{ std::move(expression) }
        , m_semicolon{ semicolon }
    {
    }
}
