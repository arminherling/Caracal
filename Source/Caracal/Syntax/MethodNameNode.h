#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/NameExpression.h>
#include <Caracal/Syntax/Token.h>
#include <Caracal/Syntax/Node.h>

namespace Caracal
{
    class CARACAL_API MethodNameNode : public Node
    {
    public:
        MethodNameNode(
            NameExpressionUPtr&& methodNameExpression);
        MethodNameNode(
            NameExpressionUPtr&& typeNameExpression,
            const Token& dotToken,
            NameExpressionUPtr&& methodNameExpression);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(MethodNameNode)
        
        [[nodiscard]] const std::optional<NameExpressionUPtr>& typeNameExpression() const noexcept { return m_typeNameExpression; }
        [[nodiscard]] const std::optional<Token>& dotToken() const noexcept { return m_dotToken; }
        [[nodiscard]] const NameExpressionUPtr& methodNameExpression() const noexcept { return m_methodNameExpression; }

    private:
        std::optional<NameExpressionUPtr> m_typeNameExpression;
        std::optional<Token> m_dotToken;
        NameExpressionUPtr m_methodNameExpression;
    };

    using MethodNameNodeUPtr = std::unique_ptr<MethodNameNode>;
}
