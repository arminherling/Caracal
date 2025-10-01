#include "Expression.h"

namespace Caracal
{
    Expression::Expression(NodeKind kind, const Type& type)
        : Node(kind, type)
    {
    }

    bool Expression::isLiteral() const noexcept
    {
        const auto thisKind = kind();
        return (thisKind == NodeKind::BoolLiteral || thisKind == NodeKind::NumberLiteral || thisKind == NodeKind::StringLiteral);
    }
}
