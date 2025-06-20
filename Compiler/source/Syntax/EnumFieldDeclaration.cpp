#include "EnumFieldDeclaration.h"

namespace Caracal
{
    EnumFieldDeclaration::EnumFieldDeclaration(
        NameExpressionUPtr&& nameExpression)
        : Node(NodeKind::EnumFieldDeclaration, Type::Undefined())
        , m_nameExpression{ std::move(nameExpression) }
        , m_colon1{ std::nullopt }
        , m_colon2{ std::nullopt }
        , m_valueExpression{ std::nullopt }
    {
    }

    EnumFieldDeclaration::EnumFieldDeclaration(
        NameExpressionUPtr&& nameExpression, 
        const Token& colon1, 
        const Token& colon2, 
        ExpressionUPtr&& valueExpression)
        : Node(NodeKind::EnumFieldDeclaration, Type::Undefined())
        , m_nameExpression{ std::move(nameExpression) }
        , m_colon1{ colon1 }
        , m_colon2{ colon2 }
        , m_valueExpression{ std::move(valueExpression) }
    {
    }
}
