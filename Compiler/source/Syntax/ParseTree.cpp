#include <Syntax/ParseTree.h>

ParseTree::ParseTree(
    const TokenBuffer& tokens, 
    std::vector<StatementUPtr>&& statements)
    : m_tokens{ tokens }
    , m_statements{ std::move(statements) }
{
}
