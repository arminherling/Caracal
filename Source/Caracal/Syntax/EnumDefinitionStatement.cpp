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
        const Token& closeBracket)
        : Statement(NodeKind::EnumDefinitionStatement, Type::Undefined())
        , m_enumKeyword{ enumKeyword }
        , m_nameToken{ nameToken }
        , m_name{ name }
        , m_colonToken{ colonToken }
        , m_baseType{ std::move(baseType) }
        , m_openBracket{ openBracket }
        , m_fieldNodes{ std::move(fieldNodes) }
        , m_closeBracket{ closeBracket }
    {
    }
}
