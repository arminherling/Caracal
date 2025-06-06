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
            ParametersNodeUPtr&& parametersNode,
            ReturnTypesNodeUPtr&& returnTypesNode,
            BlockNodeUPtr&& bodyNode);

        [[nodiscard]] const Token& keywordToken() const noexcept { return m_keywordToken; }
        [[nodiscard]] const Token& nameToken() const noexcept { return m_nameToken; }
        [[nodiscard]] const ParametersNodeUPtr& parametersNode()  const noexcept { return m_parametersNode; }
        [[nodiscard]] const ReturnTypesNodeUPtr& returnTypesNode() const noexcept { return m_returnTypesNode; }
        [[nodiscard]] const BlockNodeUPtr& bodyNode() const noexcept { return m_bodyNode; }

    private:
        Token m_keywordToken;
        Token m_nameToken;
        ParametersNodeUPtr m_parametersNode;
        ReturnTypesNodeUPtr m_returnTypesNode;
        BlockNodeUPtr m_bodyNode;
    };
    
    using FunctionDefinitionStatementUPtr = std::unique_ptr<Caracal::FunctionDefinitionStatement>;
}
