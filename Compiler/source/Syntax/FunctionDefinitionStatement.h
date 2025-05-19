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
            const Token& keyword,
            const Token& name,
            ParametersNodeUPtr&& parameters,
            ReturnTypesNodeUPtr&& returnTypes,
            BlockNodeUPtr&& body);

        [[nodiscard]] const Token& keyword() const noexcept { return m_keyword; }
        [[nodiscard]] const Token& name() const noexcept { return m_name; }
        [[nodiscard]] const ParametersNodeUPtr& parameters()  const noexcept { return m_parameters; }
        [[nodiscard]] const ReturnTypesNodeUPtr& returnTypes() const noexcept { return m_returnTypes; }
        [[nodiscard]] const BlockNodeUPtr& body() const noexcept { return m_body; }

    private:
        Token m_keyword;
        Token m_name;
        ParametersNodeUPtr m_parameters;
        ReturnTypesNodeUPtr m_returnTypes;
        BlockNodeUPtr m_body;
    };
    
    using FunctionDefinitionStatementUPtr = std::unique_ptr<Caracal::FunctionDefinitionStatement>;
}
