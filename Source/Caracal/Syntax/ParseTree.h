#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/TokenBuffer.h>
#include <Caracal/Syntax/Statement.h>

namespace Caracal
{
    class CARACAL_API ParseTree
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
