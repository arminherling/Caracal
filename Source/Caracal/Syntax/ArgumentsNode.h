#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/Token.h>
#include <Caracal/Syntax/TokenBuffer.h>
#include <vector>

namespace Caracal
{
    class CARACAL_API ArgumentsNode : public Node
    {
    public:
        ArgumentsNode(
            const Token& openParenthesisToken,
            std::vector<ExpressionUPtr>&& arguments,
            const Token& closeParenthesisToken);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(ArgumentsNode)
        
        [[nodiscard]] const Token& openParenthesisToken() const noexcept { return m_openParenthesisToken; }
        [[nodiscard]] const std::vector<ExpressionUPtr>& arguments() const noexcept { return m_arguments; }
        [[nodiscard]] const Token& closeParenthesisToken() const noexcept { return m_closeParenthesisToken; }
        [[nodiscard]] SourceLocation sourceLocation(const TokenBuffer& tokens) const;

    private:
        Token m_openParenthesisToken;
        std::vector<ExpressionUPtr> m_arguments;
        Token m_closeParenthesisToken;
    };

    using ArgumentsNodeUPtr = std::unique_ptr<ArgumentsNode>;
}
