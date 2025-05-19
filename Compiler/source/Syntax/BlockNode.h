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
            const Token& openBracket,
            std::vector<StatementUPtr>&& statements,
            const Token& closeBracket);

        BlockNode(const BlockNode&) = delete;
        BlockNode& operator=(const BlockNode&) = delete;

        BlockNode(BlockNode&&) = default;
        BlockNode& operator=(BlockNode&&) = default;

        [[nodiscard]] const Token& openBracket() const noexcept { return m_openBracket; }
        [[nodiscard]] const std::vector<StatementUPtr>& statements() const noexcept { return m_statements; }
        [[nodiscard]] const Token& closeBracket() const noexcept { return m_closeBracket; }

    private:
        Token m_openBracket;
        std::vector<StatementUPtr> m_statements;
        Token m_closeBracket;
    };

    using BlockNodeUPtr = std::unique_ptr<BlockNode>;
}
