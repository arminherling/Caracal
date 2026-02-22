#include "EnumDefinitionStatement.h"

namespace Caracal 
{
    EnumDefinitionStatement::EnumDefinitionStatement(
        const Token& enumKeyword, 
        const Token& nameToken,
        std::string_view name,
        const std::optional<Token>& colonToken, 
        std::optional<TypeNameNodeUPtr>&& baseType, 
        const Token& openBracket, 
        std::vector<EnumFieldDeclarationUPtr>&& fieldNodes, 
        const Token& closeBracket,
        std::optional<AnnotationNodeUPtr>&& annotation)
        : Statement(NodeKind::EnumDefinitionStatement, Type::Undefined())
        , m_enumKeyword{ enumKeyword }
        , m_nameToken{ nameToken }
        , m_name{ name }
        , m_colonToken{ colonToken }
        , m_baseType{ std::move(baseType) }
        , m_openBracket{ openBracket }
        , m_fieldNodes{ std::move(fieldNodes) }
        , m_closeBracket{ closeBracket }
        , m_annotation{ std::move(annotation) }
    {
    }

    bool EnumDefinitionStatement::hasStep() const noexcept
    {
        if (m_annotation.has_value())
        {
            const auto& annotationNode = m_annotation.value();
            return annotationNode->kind() == AnnotationKind::Step;
        }
        return false;
    }

    bool EnumDefinitionStatement::isFlag() const noexcept
    {
        if (m_annotation.has_value())
        {
            const auto& annotationNode = m_annotation.value();
            return annotationNode->kind() == AnnotationKind::Flag;
        }
        return false;
    }
}
