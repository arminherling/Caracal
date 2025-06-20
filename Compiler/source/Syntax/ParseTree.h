#pragma once

#include <Compiler/API.h>
#include <Syntax/TokenBuffer.h>
#include <Syntax/Statement.h>

namespace Caracal
{
    class COMPILER_API ParseTree
    {
    public:
        ParseTree(
            const TokenBuffer& tokens,
            std::vector<StatementUPtr>&& statements);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(ParseTree)

        [[nodiscard]] const std::vector<StatementUPtr>& statements() const noexcept { return m_statements; };
        [[nodiscard]] const TokenBuffer& tokens() const noexcept { return m_tokens; }

    private:
        TokenBuffer m_tokens;
        std::vector<StatementUPtr> m_statements;
    };
}
