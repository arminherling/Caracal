#pragma once

#include <Compiler/API.h>
#include <Syntax/Token.h>
#include <Syntax/Statement.h>
#include <vector>

namespace Caracal 
{
    class COMPILER_API BlockNode : public Statement
    {
    public:
        BlockNode(
            const Token& openBracketToken,
            std::vector<StatementUPtr>&& statements,
            const Token& closeBracketToken);

        BlockNode(const BlockNode&) = delete;
        BlockNode& operator=(const BlockNode&) = delete;

        BlockNode(BlockNode&&) = default;
        BlockNode& operator=(BlockNode&&) = default;

        [[nodiscard]] const Token& openBracketToken() const noexcept { return m_openBracketToken; }
        [[nodiscard]] const std::vector<StatementUPtr>& statements() const noexcept { return m_statements; }
        [[nodiscard]] const Token& closeBracketToken() const noexcept { return m_closeBracketToken; }

    private:
        Token m_openBracketToken;
        std::vector<StatementUPtr> m_statements;
        Token m_closeBracketToken;
    };

    using BlockNodeUPtr = std::unique_ptr<BlockNode>;
}
