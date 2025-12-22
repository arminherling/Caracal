#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/EnumFieldDeclaration.h>
#include <Caracal/Syntax/Statement.h>
#include <Caracal/Syntax/Token.h>
#include <Caracal/Syntax/TypeNameNode.h>

namespace Caracal
{
    class CARACAL_API EnumDefinitionStatement : public Statement
    {
    public:
        EnumDefinitionStatement(
            const Token& enumKeyword,
            NameExpressionUPtr&& nameExpression,
            const std::optional<Token>& colonToken,
            std::optional<TypeNameNodeUPtr>&& baseType,
            const Token& openBracket,
            std::vector<EnumFieldDeclarationUPtr>&& fieldNodes,
            const Token& closeBracket);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(EnumDefinitionStatement)

        [[nodiscard]] const Token& enumKeyword() const noexcept { return m_enumKeyword; }
        [[nodiscard]] const NameExpressionUPtr& nameExpression() const noexcept { return m_nameExpression; }
        [[nodiscard]] const std::optional<Token>& colonToken() const noexcept { return m_colonToken; }
        [[nodiscard]] const std::optional<TypeNameNodeUPtr>& baseType() const noexcept { return m_baseType; }
        [[nodiscard]] const Token& openBracket() const noexcept { return m_openBracket; }
        [[nodiscard]] const std::vector<EnumFieldDeclarationUPtr>& fieldNodes() const noexcept { return m_fieldNodes; }
        [[nodiscard]] const Token& closeBracket() const noexcept { return m_closeBracket; }

    private:
        Token m_enumKeyword;
        NameExpressionUPtr m_nameExpression;
        std::optional<Token> m_colonToken;
        std::optional<TypeNameNodeUPtr> m_baseType;
        Token m_openBracket;
        std::vector<EnumFieldDeclarationUPtr> m_fieldNodes;
        Token m_closeBracket;
    };
}
