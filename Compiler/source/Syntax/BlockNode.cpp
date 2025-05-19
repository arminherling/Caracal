#include "BlockNode.h"

namespace Caracal 
{
    BlockNode::BlockNode(const Token& openBracket, std::vector<StatementUPtr>&& statements, const Token& closeBracket)
        : Statement(NodeKind::BlockNode, Type::Undefined())
        , m_openBracket{ openBracket }
        , m_statements{ std::move(statements) }
        , m_closeBracket{ closeBracket }
    {
    }
}
