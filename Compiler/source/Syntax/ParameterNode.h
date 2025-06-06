#pragma once

#include <Compiler/API.h>
#include <Syntax/Node.h>
#include <Syntax/NameExpression.h>
#include <Syntax/TypeNameNode.h>
#include <Syntax/Token.h>
#include <memory>

namespace Caracal
{
    class COMPILER_API ParameterNode : public Node
    {
    public:
        ParameterNode(
            NameExpressionUPtr&& nameExpression,
            const Token& colonToken,
            TypeNameNodeUPtr&& typeName);

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
