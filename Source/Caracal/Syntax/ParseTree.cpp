#include <Caracal/Syntax/ParseTree.h>

namespace Caracal
{
    ParseTree::ParseTree(
        TokenBuffer tokens,
        std::vector<StatementUPtr>&& statements)
        : m_tokens{ std::move(tokens) }
        , m_statements{ std::move(statements) }
    {
    }
}
