#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Statement.h>
#include <Caracal/Syntax/AnnotationNode.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/TypeNameNode.h>
#include <Caracal/Syntax/Token.h>

#include <vector>

namespace Caracal
{
    class CARACAL_API ConstantDeclaration : public Statement
    {
    public:
        ConstantDeclaration(
            ExpressionUPtr&& leftExpression,
            const Token& firstColonToken,
            std::optional<TypeNameNodeUPtr>&& explicitType,
            const Token& secondColonToken,
            ExpressionUPtr&& rightExpression,
            const Token& semicolonToken,
            bool isGlobalConstant,
            std::vector<AnnotationNodeUPtr>&& annotations = {});

        ConstantDeclaration(
            ExpressionUPtr&& leftExpression,
            const Token& firstColonToken,
            const Token& secondColonToken,
            const Token& initKeywordToken,
            TypeNameNodeUPtr&& initType,
            const Token& semicolonToken,
            bool isGlobalConstant,
            std::vector<AnnotationNodeUPtr>&& annotations = {});

        CARACAL_DELETE_COPY_DEFAULT_MOVE(ConstantDeclaration)

        [[nodiscard]] const ExpressionUPtr& leftExpression() const noexcept { return m_leftExpression; }
        [[nodiscard]] const Token& firstColonToken() const noexcept { return m_firstColonToken; }
        [[nodiscard]] const std::optional<TypeNameNodeUPtr>& explicitType() const noexcept { return m_explicitType; }
        [[nodiscard]] const Token& secondColonToken() const noexcept { return m_secondColonToken; }
        [[nodiscard]] const ExpressionUPtr& rightExpression() const noexcept { return m_rightExpression; }
        [[nodiscard]] const Token& semicolonToken() const noexcept { return m_semicolonToken; }
        [[nodiscard]] bool isGlobalConstant() const noexcept { return m_isGlobalConstant; }
        [[nodiscard]] bool isInit() const noexcept { return m_isInit; }
        [[nodiscard]] const Token& initKeywordToken() const noexcept { return m_initKeywordToken; }
        [[nodiscard]] const std::vector<AnnotationNodeUPtr>& annotations() const noexcept { return m_annotations; }

    private:
        ExpressionUPtr m_leftExpression;
        Token m_firstColonToken;
        std::optional<TypeNameNodeUPtr> m_explicitType;
        Token m_secondColonToken;
        ExpressionUPtr m_rightExpression;
        Token m_semicolonToken;
        bool m_isGlobalConstant;
        bool m_isInit{ false };
        Token m_initKeywordToken;
        std::vector<AnnotationNodeUPtr> m_annotations;
    };
}
