#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/NameExpression.h>
#include <Caracal/Syntax/ArgumentsNode.h>

namespace Caracal
{
    class CARACAL_API AnnotationNode : public Node
    {
    public:
        AnnotationNode(
            const Token& hashToken,
            const Token& nameToken,
            std::string_view name, 
            ArgumentsNodeUPtr&& argumentsNode);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(AnnotationNode)

        [[nodiscard]] const Token& hashToken() const noexcept { return m_hashToken; }
        [[nodiscard]] const Token& nameToken() const noexcept { return m_nameToken; }
        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        [[nodiscard]] const ArgumentsNodeUPtr& argumentsNode() const noexcept { return m_argumentsNode; }

    private:
        Token m_hashToken;
        Token m_nameToken;
        std::string m_name;
        ArgumentsNodeUPtr m_argumentsNode;
    };

    using AnnotationNodeUPtr = std::unique_ptr<AnnotationNode>;
}
