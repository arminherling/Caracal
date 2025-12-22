#include "TypeFieldDeclaration.h"

namespace Caracal 
{
    TypeFieldDeclaration::TypeFieldDeclaration(
        NameExpressionUPtr&& nameExpression,
        const Token& firstColonToken,
        std::optional<TypeNameNodeUPtr>&& explicitType,
        const std::optional<Token>& secondToken,
        std::optional<ExpressionUPtr>&& rightExpression,
        bool isConstant)
        : Statement(NodeKind::TypeFieldDeclaration, (rightExpression.has_value() ? rightExpression.value()->type() : Type::Undefined()))
        , m_nameExpression{ std::move(nameExpression) }
        , m_firstColonToken{ firstColonToken }
        , m_explicitType{ std::move(explicitType) }
        , m_secondToken{ secondToken }
        , m_rightExpression{ std::move(rightExpression) }
        , m_isConstant{ isConstant }
    {
    }
}