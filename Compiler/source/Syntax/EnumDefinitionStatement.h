#pragma once

#include <Compiler/API.h>
#include <Syntax/EnumFieldNode.h>
#include <Syntax/Statement.h>
#include <Syntax/Token.h>
#include <Syntax/TypeNameNode.h>

namespace Caracal
{
    class COMPILER_API EnumDefinitionStatement : public Statement
    {
    public:
        EnumDefinitionStatement(
            const Token& enumKeyword,
            NameExpressionUPtr&& nameExpression,
            const std::optional<Token>& colonToken,
            std::optional<TypeNameNodeUPtr>&& baseType,
            const Token& openBracket,
            std::vector<EnumFieldNodeUPtr>&& fieldNodes,
            const Token& closeBracket);

        EnumDefinitionStatement(const EnumDefinitionStatement&) = delete;
        EnumDefinitionStatement& operator=(const EnumDefinitionStatement&) = delete;
        
        EnumDefinitionStatement(EnumDefinitionStatement&&) = default;
        EnumDefinitionStatement& operator=(EnumDefinitionStatement&&) = default;

        [[nodiscard]] const Token& enumKeyword() const noexcept { return m_enumKeyword; }
        [[nodiscard]] const NameExpressionUPtr& nameExpression() const noexcept { return m_nameExpression; }
        [[nodiscard]] const std::optional<Token>& colonToken() const noexcept { return m_colonToken; }
        [[nodiscard]] const std::optional<TypeNameNodeUPtr>& baseType() const noexcept { return m_baseType; }
        [[nodiscard]] const Token& openBracket() const noexcept { return m_openBracket; }
        [[nodiscard]] const std::vector<EnumFieldNodeUPtr>& fieldNodes() const noexcept { return m_fieldNodes; }
        [[nodiscard]] const Token& closeBracket() const noexcept { return m_closeBracket; }

    private:
        Token m_enumKeyword;
        NameExpressionUPtr m_nameExpression;
        std::optional<Token> m_colonToken;
        std::optional<TypeNameNodeUPtr> m_baseType;
        Token m_openBracket;
        std::vector<EnumFieldNodeUPtr> m_fieldNodes;
        Token m_closeBracket;
    };
}
