#pragma once

#include <Compiler/API.h>
#include <Syntax/Statement.h>
#include <Syntax/Token.h>
#include <Syntax/MethodNameNode.h>
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

    enum class SpecialFunctionType
    {
        None,
        Constructor,
        Destructor
    };

    class COMPILER_API MethodDefinitionStatement : public Statement
    {
    public:
        MethodDefinitionStatement(
            const Token& keywordToken,
            MethodNameNodeUPtr&& methodNameNode,
            ParametersNodeUPtr&& parametersNode,
            ReturnTypesNodeUPtr&& returnTypesNode,
            BlockNodeUPtr&& bodyNode,
            MethodModifier modifier,
            SpecialFunctionType specialFunctionType);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(MethodDefinitionStatement)

        [[nodiscard]] const Token& keywordToken() const noexcept { return m_keywordToken; }
        [[nodiscard]] const MethodNameNodeUPtr& methodNameNode() const noexcept { return m_methodNameNode; }
        [[nodiscard]] const ParametersNodeUPtr& parametersNode() const noexcept { return m_parametersNode; }
        [[nodiscard]] const ReturnTypesNodeUPtr& returnTypesNode() const noexcept { return m_returnTypesNode; }
        [[nodiscard]] const BlockNodeUPtr& bodyNode() const noexcept { return m_bodyNode; }
        [[nodiscard]] MethodModifier modifier() const noexcept { return m_modifier; }
        [[nodiscard]] SpecialFunctionType specialFunctionType() const noexcept { return m_specialFunctionType; }


    private:
        Token m_keywordToken;
        MethodNameNodeUPtr m_methodNameNode;
        ParametersNodeUPtr m_parametersNode;
        ReturnTypesNodeUPtr m_returnTypesNode;
        BlockNodeUPtr m_bodyNode;
        MethodModifier m_modifier;
        SpecialFunctionType m_specialFunctionType;
    };

    [[nodiscard]] COMPILER_API QString stringify(MethodModifier modifier);
    [[nodiscard]] COMPILER_API QString stringify(SpecialFunctionType specialFunctionType);
}
