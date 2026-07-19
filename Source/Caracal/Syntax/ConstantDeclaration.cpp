#include "ConstantDeclaration.h"

namespace Caracal
{
    ConstantDeclaration::ConstantDeclaration(
        ExpressionUPtr&& leftExpression,
        const Token& firstColonToken,
        std::optional<TypeNameNodeUPtr>&& explicitType,
        const Token& secondColonToken,
        ExpressionUPtr&& rightExpression,
        const Token& semicolonToken,
        bool isGlobalConstant,
        std::vector<AnnotationNodeUPtr>&& annotations)
        : Statement(NodeKind::ConstantDeclaration, rightExpression->type())
        , m_leftExpression{ std::move(leftExpression) }
        , m_firstColonToken{ firstColonToken }
        , m_explicitType{ std::move(explicitType) }
        , m_secondColonToken{ secondColonToken }
        , m_rightExpression{ std::move(rightExpression) }
        , m_semicolonToken{ semicolonToken }
        , m_isGlobalConstant{ isGlobalConstant }
        , m_annotations{ std::move(annotations) }
    {
    }

    ConstantDeclaration::ConstantDeclaration(
        ExpressionUPtr&& leftExpression,
        const Token& firstColonToken,
        const Token& secondColonToken,
        const Token& initKeywordToken,
        TypeNameNodeUPtr&& initType,
        const Token& semicolonToken,
        bool isGlobalConstant,
        std::vector<AnnotationNodeUPtr>&& annotations)
        : Statement(NodeKind::ConstantDeclaration, Type::Undefined())
        , m_leftExpression{ std::move(leftExpression) }
        , m_firstColonToken{ firstColonToken }
        , m_explicitType{ std::move(initType) }
        , m_secondColonToken{ secondColonToken }
        , m_rightExpression{ nullptr }
        , m_semicolonToken{ semicolonToken }
        , m_isGlobalConstant{ isGlobalConstant }
        , m_isInit{ true }
        , m_initKeywordToken{ initKeywordToken }
        , m_annotations{ std::move(annotations) }
    {
    }
}
