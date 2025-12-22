#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Node.h>
#include <Caracal/Syntax/NameExpression.h>
#include <Caracal/Syntax/TypeNameNode.h>
#include <Caracal/Syntax/Token.h>
#include <memory>

namespace Caracal
{
    class CARACAL_API ParameterNode : public Node
    {
    public:
        ParameterNode(
            NameExpressionUPtr&& nameExpression,
            const Token& colonToken,
            TypeNameNodeUPtr&& typeName);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(ParameterNode)

        [[nodiscard]] const NameExpressionUPtr& nameExpression() const noexcept { return m_nameExpression; }
        [[nodiscard]] const Token& colonToken() const noexcept { return m_colonToken; }
        [[nodiscard]] const TypeNameNodeUPtr& typeName() const noexcept { return m_typeName; }

    private:
        NameExpressionUPtr m_nameExpression;
        Token m_colonToken;
        TypeNameNodeUPtr m_typeName;
    };

    using ParameterNodeUPtr = std::unique_ptr<ParameterNode>;
}
