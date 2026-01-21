#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/BlockNode.h>
#include <Caracal/Syntax/Statement.h>
#include <Caracal/Syntax/NameExpression.h>
#include <Caracal/Syntax/ParametersNode.h>
#include <Caracal/Syntax/Token.h>

namespace Caracal
{
    class CARACAL_API TypeDefinitionStatement : public Statement
    {
    public:
        TypeDefinitionStatement(
            const Token& typeKeyword, 
            const Token& nameToken,
            std::string_view name,
            std::optional<ParametersNodeUPtr>&& constructorParameters,
            BlockNodeUPtr&& bodyNode);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(TypeDefinitionStatement)

        [[nodiscard]] const Token& typeKeyword() const noexcept { return m_typeKeyword; }
        [[nodiscard]] const Token& nameToken() const noexcept { return m_nameToken; }
        [[nodiscard]] std::string_view name() const noexcept { return m_name; }
        [[nodiscard]] const std::optional<ParametersNodeUPtr>& constructorParameters() const noexcept { return m_constructorParameters; }
        [[nodiscard]] const BlockNodeUPtr& bodyNode() const noexcept { return m_bodyNode; }

    private:
        Token m_typeKeyword;
        Token m_nameToken;
        std::string m_name;
        std::optional<ParametersNodeUPtr> m_constructorParameters;
        BlockNodeUPtr m_bodyNode;
    };
}
