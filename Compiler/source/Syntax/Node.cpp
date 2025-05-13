#include <Syntax/Node.h>

Node::Node(NodeKind kind, const Type& type)
    : m_kind{ kind }
    , m_type{ type }
{
}
