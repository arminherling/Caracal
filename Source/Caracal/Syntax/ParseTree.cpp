#include <Caracal/Syntax/ParseTree.h>

namespace Caracal
{
    ParseTree::ParseTree(
        const TokenBuffer& tokens,
        std::vector<StatementUPtr>&& statements)
        : m_tokens{ tokens }
        , m_statements{ std::move(statements) }
    {
    }
}
