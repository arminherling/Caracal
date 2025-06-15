#pragma once

#include <Compiler/API.h>
#include <Syntax/Expression.h>
#include <Syntax/Token.h>
#include <vector>

namespace Caracal
{
    class COMPILER_API ArgumentsNode : public Node
    {
    public:
        ArgumentsNode(
            const Token& openParenthesisToken,
            std::vector<ExpressionUPtr>&& arguments,
            const Token& closeParenthesisToken);
        ArgumentsNode(const ArgumentsNode&) = delete;
        ArgumentsNode& operator=(const ArgumentsNode&) = delete;
        
        ArgumentsNode(ArgumentsNode&&) = default;
        ArgumentsNode& operator=(ArgumentsNode&&) = default;
        
        [[nodiscard]] const Token& openParenthesisToken() const noexcept { return m_openParenthesisToken; }
        [[nodiscard]] const std::vector<ExpressionUPtr>& arguments() const noexcept { return m_arguments; }
        [[nodiscard]] const Token& closeParenthesisToken() const noexcept { return m_closeParenthesisToken; }

    private:
        Token m_openParenthesisToken;
        std::vector<ExpressionUPtr> m_arguments;
        Token m_closeParenthesisToken;
    };

    using ArgumentsNodeUPtr = std::unique_ptr<ArgumentsNode>;
}
