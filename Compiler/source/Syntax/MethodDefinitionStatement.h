#pragma once

#include <Compiler/API.h>
#include <Syntax/Statement.h>
#include <Syntax/Token.h>
#include <Syntax/ParametersNode.h>
#include <Syntax/ReturnTypesNode.h>
#include <Syntax/BlockNode.h>

namespace Caracal
{
    enum class MethodModifier
    {
        Public,
        Private,
        Static
    };

    class COMPILER_API MethodDefinitionStatement : public Statement
    {
    public:
        MethodDefinitionStatement(
            const Token& keywordToken,
            NameExpressionUPtr&& nameExpression,
            ParametersNodeUPtr&& parametersNode,
            ReturnTypesNodeUPtr&& returnTypesNode,
            BlockNodeUPtr&& bodyNode,
            MethodModifier modifier);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(MethodDefinitionStatement)

        [[nodiscard]] const Token& keywordToken() const noexcept { return m_keywordToken; }
        [[nodiscard]] const NameExpressionUPtr& nameExpression() const noexcept { return m_nameExpression; }
        [[nodiscard]] const ParametersNodeUPtr& parametersNode() const noexcept { return m_parametersNode; }
        [[nodiscard]] const ReturnTypesNodeUPtr& returnTypesNode() const noexcept { return m_returnTypesNode; }
        [[nodiscard]] const BlockNodeUPtr& bodyNode() const noexcept { return m_bodyNode; }
        [[nodiscard]] MethodModifier modifier() const noexcept { return m_modifier; }

    private:
        Token m_keywordToken;
        NameExpressionUPtr m_nameExpression;
        ParametersNodeUPtr m_parametersNode;
        ReturnTypesNodeUPtr m_returnTypesNode;
        BlockNodeUPtr m_bodyNode;
        MethodModifier m_modifier;
    };

    [[nodiscard]] COMPILER_API QString stringify(MethodModifier modifier);
}
