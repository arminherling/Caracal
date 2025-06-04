#include "ConstantDeclaration.h"

namespace Caracal
{
    ConstantDeclaration::ConstantDeclaration(
        ExpressionUPtr&& leftExpression,
        const Token& firstColon,
        const Token& secondColon,
        ExpressionUPtr&& rightExpression,
        const Token& semicolon)
        : Statement(NodeKind::ConstantDeclaration, rightExpression->type())
        , m_leftExpression{ std::move(leftExpression) }
        , m_firstColon{ firstColon }
        , m_secondColon{ secondColon }
        , m_rightExpression{ std::move(rightExpression) }
        , m_semicolon{ semicolon }
    {
    }
}
