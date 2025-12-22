#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/NameExpression.h>
#include <Caracal/Syntax/ArgumentsNode.h>

namespace Caracal
{
    class CARACAL_API FunctionCallExpression : public Expression
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
