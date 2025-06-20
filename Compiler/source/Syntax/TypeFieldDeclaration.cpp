#include "TypeFieldDeclaration.h"

namespace Caracal {
    TypeFieldDeclaration::TypeFieldDeclaration(
        NameExpressionUPtr&& nameExpression,
        const Token& firstColonToken,
        const Token& secondToken,
        ExpressionUPtr&& rightExpression,
        bool isConstant)
        : Statement(NodeKind::TypeFieldDeclaration, rightExpression->type())
        , m_nameExpression{ std::move(nameExpression) }
        , m_firstColonToken{ firstColonToken }
        , m_secondToken{ secondToken }
        , m_rightExpression{ std::move(rightExpression) }
        , m_isConstant{ isConstant }
    {
    }
}