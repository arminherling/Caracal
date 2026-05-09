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
        std::vector<AnnotationNodeUPtr>&& annotations)
        : Statement(NodeKind::EnumDefinitionStatement, Type::Undefined())
        , m_enumKeyword{ enumKeyword }
        , m_nameToken{ nameToken }
        , m_name{ name }
        , m_colonToken{ colonToken }
        , m_baseType{ std::move(baseType) }
        , m_openBracket{ openBracket }
        , m_fieldNodes{ std::move(fieldNodes) }
        , m_closeBracket{ closeBracket }
        , m_annotations{ std::move(annotations) }
    {
    }

    bool EnumDefinitionStatement::hasStep() const noexcept
    {
        for (const auto& annotation : m_annotations)
        {
            if (annotation->kind() == AnnotationKind::Step)
            {
                return true;
            }
        }

        return false;
    }

    bool EnumDefinitionStatement::isFlag() const noexcept
    {
        for (const auto& annotation : m_annotations)
        {
            if (annotation->kind() == AnnotationKind::Flag)
            {
                return true;
            }
        }

        return false;
    }
}
