#include "Expression.h"

namespace Caracal
{
    Expression::Expression(NodeKind kind, const Type& type)
        : Node(kind, type)
    {
    }
}
