#pragma once

#include <Compiler/API.h>
#include <Syntax/Statement.h>
#include <Syntax/Token.h>
#include <Syntax/ParametersNode.h>
#include <Syntax/ReturnTypesNode.h>
#include <Syntax/BlockNode.h>

namespace Caracal
{
    class COMPILER_API FunctionDefinitionStatement : public Statement
    {
    public:
        FunctionDefinitionStatement(
            const Token& keywordToken,
            const Token& nameToken,
            ParametersNodeUPtr&& parameters,
            ReturnTypesNodeUPtr&& returnTypes,
            BlockNodeUPtr&& body);

        [[nodiscard]] const Token& keywordToken() const noexcept { return m_keywordToken; }
        [[nodiscard]] const Token& nameToken() const noexcept { return m_nameToken; }
        [[nodiscard]] const ParametersNodeUPtr& parameters()  const noexcept { return m_parameters; }
        [[nodiscard]] const ReturnTypesNodeUPtr& returnTypes() const noexcept { return m_returnTypes; }
        [[nodiscard]] const BlockNodeUPtr& body() const noexcept { return m_body; }

    private:
        Token m_keywordToken;
        Token m_nameToken;
        ParametersNodeUPtr m_parameters;
        ReturnTypesNodeUPtr m_returnTypes;
        BlockNodeUPtr m_body;
    };
    
    using FunctionDefinitionStatementUPtr = std::unique_ptr<Caracal::FunctionDefinitionStatement>;
}
