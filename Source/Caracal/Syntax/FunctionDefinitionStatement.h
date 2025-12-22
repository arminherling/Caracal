#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Statement.h>
#include <Caracal/Syntax/Token.h>
#include <Caracal/Syntax/ParametersNode.h>
#include <Caracal/Syntax/ReturnTypesNode.h>
#include <Caracal/Syntax/BlockNode.h>

namespace Caracal
{
    class CARACAL_API FunctionDefinitionStatement : public Statement
    {
    public:
        FunctionDefinitionStatement(
            const Token& keywordToken,
            NameExpressionUPtr&& nameExpression,
            ParametersNodeUPtr&& parametersNode,
            ReturnTypesNodeUPtr&& returnTypesNode,
            BlockNodeUPtr&& bodyNode);
        
        CARACAL_DELETE_COPY_DEFAULT_MOVE(FunctionDefinitionStatement)

        [[nodiscard]] const Token& keywordToken() const noexcept { return m_keywordToken; }
        [[nodiscard]] const NameExpressionUPtr& nameExpression() const noexcept { return m_nameExpression; }
        [[nodiscard]] const ParametersNodeUPtr& parametersNode()  const noexcept { return m_parametersNode; }
        [[nodiscard]] const ReturnTypesNodeUPtr& returnTypesNode() const noexcept { return m_returnTypesNode; }
        [[nodiscard]] const BlockNodeUPtr& bodyNode() const noexcept { return m_bodyNode; }

    private:
        Token m_keywordToken;
        NameExpressionUPtr m_nameExpression;
        ParametersNodeUPtr m_parametersNode;
        ReturnTypesNodeUPtr m_returnTypesNode;
        BlockNodeUPtr m_bodyNode;
    };
}
