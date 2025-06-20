#pragma once

#include <Compiler/API.h>
#include <Syntax/NameExpression.h>
#include <Syntax/ArgumentsNode.h>

namespace Caracal
{
    class COMPILER_API FunctionCallExpression : public Expression
    {
    public:
        FunctionCallExpression(
            NameExpressionUPtr&& nameExpression,
            ArgumentsNodeUPtr&& argumentsNode);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(FunctionCallExpression)

        [[nodiscard]] const NameExpressionUPtr& nameExpression() const noexcept { return m_nameExpression; }
        [[nodiscard]] const ArgumentsNodeUPtr& argumentsNode() const noexcept { return m_argumentsNode; }

    private:
        NameExpressionUPtr m_nameExpression;
        ArgumentsNodeUPtr m_argumentsNode;
    };
}
