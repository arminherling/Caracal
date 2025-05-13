#pragma once

#include <Compiler/API.h>
#include <Syntax/TokenBuffer.h>
#include <Syntax/Node.h>
#include <Syntax/Statement.h>

#include <QList>

class COMPILER_API ParseTree
{
public:
    ParseTree(
        const TokenBuffer& tokens, 
        std::vector<StatementUPtr>&& statements);

    ParseTree(const ParseTree&) = delete;
    ParseTree& operator=(const ParseTree&) = delete;

    ParseTree(ParseTree&&) = default;
    ParseTree& operator=(ParseTree&&) = default;

    [[nodiscard]] const std::vector<StatementUPtr>& statements() const noexcept { return m_statements; };
    [[nodiscard]] const TokenBuffer& tokens() const noexcept { return m_tokens; }

private:
    TokenBuffer m_tokens;
    std::vector<StatementUPtr> m_statements;
};
