#include "EnumFieldDeclaration.h"

namespace Caracal
{
    EnumFieldDeclaration::EnumFieldDeclaration(
        const Token& nameToken,
        std::string_view name)
        : Node(NodeKind::EnumFieldDeclaration, Type::Undefined())
        , m_nameToken{ nameToken }
        , m_name{ name }
        , m_colon1{ std::nullopt }
        , m_colon2{ std::nullopt }
        , m_valueExpression{ std::nullopt }
    {
    }

    EnumFieldDeclaration::EnumFieldDeclaration(
        const Token& nameToken,
        std::string_view name,
        const Token& colon1,
        const Token& colon2,
        ExpressionUPtr&& valueExpression)
        : Node(NodeKind::EnumFieldDeclaration, Type::Undefined())
        , m_nameToken{ nameToken }
        , m_name{ name }
        , m_colon1{ colon1 }
        , m_colon2{ colon2 }
        , m_valueExpression{ std::move(valueExpression) }
    {
    }
}
