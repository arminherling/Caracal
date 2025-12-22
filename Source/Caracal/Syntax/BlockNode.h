#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Token.h>
#include <Caracal/Syntax/Statement.h>
#include <vector>

namespace Caracal 
{
    class CARACAL_API BlockNode : public Statement
    {
    public:
        BlockNode(
            const Token& openBracketToken,
            std::vector<StatementUPtr>&& statements,
            const Token& closeBracketToken);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(BlockNode)

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
