#include "Statement.h"

namespace Caracal
{
    Statement::Statement(NodeKind kind, const Type& type)
        : Node(kind, type)
    {
    }
}
