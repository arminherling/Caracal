#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/EnumFieldDeclaration.h>
#include <Caracal/Syntax/Statement.h>
#include <Caracal/Syntax/Token.h>
#include <Caracal/Syntax/TypeNameNode.h>
#include <Caracal/Syntax/AnnotationNode.h>

#include <vector>

namespace Caracal
{
    class CARACAL_API EnumDefinitionStatement : public Statement
    {
    public:
        EnumDefinitionStatement(
            const Token& enumKeyword,
            const Token& nameToken,
            std::string_view name,
            const std::optional<Token>& colonToken,
            std::optional<TypeNameNodeUPtr>&& baseType,
            const Token& openBracket,
            std::vector<EnumFieldDeclarationUPtr>&& fieldNodes,
            const Token& closeBracket,
            std::vector<AnnotationNodeUPtr>&& annotations);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(EnumDefinitionStatement)

        [[nodiscard]] const Token& enumKeyword() const noexcept { return m_enumKeyword; }
        [[nodiscard]] const Token& nameToken() const noexcept { return m_nameToken; }
        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        [[nodiscard]] const std::optional<Token>& colonToken() const noexcept { return m_colonToken; }
        [[nodiscard]] const std::optional<TypeNameNodeUPtr>& baseType() const noexcept { return m_baseType; }
        [[nodiscard]] const Token& openBracket() const noexcept { return m_openBracket; }
        [[nodiscard]] const std::vector<EnumFieldDeclarationUPtr>& fieldNodes() const noexcept { return m_fieldNodes; }
        [[nodiscard]] const Token& closeBracket() const noexcept { return m_closeBracket; }
        [[nodiscard]] const std::vector<AnnotationNodeUPtr>& annotations() const noexcept { return m_annotations; }
        [[nodiscard]] bool hasStep() const noexcept;
        [[nodiscard]] bool isFlag() const noexcept;

    private:
        Token m_enumKeyword;
        Token m_nameToken;
        std::string m_name;
        std::optional<Token> m_colonToken;
        std::optional<TypeNameNodeUPtr> m_baseType;
        Token m_openBracket;
        std::vector<EnumFieldDeclarationUPtr> m_fieldNodes;
        Token m_closeBracket;
        std::vector<AnnotationNodeUPtr> m_annotations;
    };
}
