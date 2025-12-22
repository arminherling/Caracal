#include "BlockNode.h"

namespace Caracal 
{
    BlockNode::BlockNode(
        const Token& openBracketToken, 
        std::vector<StatementUPtr>&& statements, 
        const Token& closeBracketToken)
        : Statement(NodeKind::BlockNode, Type::Undefined())
        , m_openBracketToken{ openBracketToken }
        , m_statements{ std::move(statements) }
        , m_closeBracketToken{ closeBracketToken }
    {
    }
}
