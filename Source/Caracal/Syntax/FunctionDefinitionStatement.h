#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Statement.h>
#include <Caracal/Syntax/Token.h>
#include <Caracal/Syntax/ParametersNode.h>
#include <Caracal/Syntax/ReturnTypesNode.h>
#include <Caracal/Syntax/BlockNode.h>
#include <Caracal/Syntax/AnnotationNode.h>

namespace Caracal
{
    class CARACAL_API FunctionDefinitionStatement : public Statement
    {
    public:
        FunctionDefinitionStatement(
            const Token& keywordToken,
            const Token& nameToken,
            std::string_view name,
            ParametersNodeUPtr&& parametersNode,
            ReturnTypesNodeUPtr&& returnTypesNode,
            BlockNodeUPtr&& bodyNode,
            std::optional<AnnotationNodeUPtr>&& annotationNode);
        
        CARACAL_DELETE_COPY_DEFAULT_MOVE(FunctionDefinitionStatement)

        [[nodiscard]] const Token& keywordToken() const noexcept { return m_keywordToken; }
        [[nodiscard]] const Token& nameToken() const noexcept { return m_nameToken; }
        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        [[nodiscard]] const ParametersNodeUPtr& parametersNode()  const noexcept { return m_parametersNode; }
        [[nodiscard]] const ReturnTypesNodeUPtr& returnTypesNode() const noexcept { return m_returnTypesNode; }
        [[nodiscard]] const BlockNodeUPtr& bodyNode() const noexcept { return m_bodyNode; }
        [[nodiscard]] const std::optional<AnnotationNodeUPtr>& annotationNode() const noexcept { return m_annotationNode; }
        [[nodiscard]] bool isExtern() const noexcept { return (m_annotationNode.has_value() && m_annotationNode->get()->name() == "extern"); }

    private:
        Token m_keywordToken;
        Token m_nameToken;
        std::string m_name;
        ParametersNodeUPtr m_parametersNode;
        ReturnTypesNodeUPtr m_returnTypesNode;
        BlockNodeUPtr m_bodyNode;
        std::optional<AnnotationNodeUPtr> m_annotationNode;
    };
}
