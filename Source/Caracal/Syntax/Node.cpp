#include <Caracal/Syntax/Node.h>

namespace Caracal
{
    Node::Node(NodeKind kind, const Type& type)
        : m_kind{ kind }
        , m_type{ type }
    {
    }
}
