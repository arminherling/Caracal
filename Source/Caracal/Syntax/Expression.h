#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Node.h>

namespace Caracal
{
    class CARACAL_API Expression : public Node
    {
    public:
        Expression(NodeKind kind, const Type& type);

        [[nodiscard]] bool isLiteral() const noexcept;
    };

    using ExpressionUPtr = std::unique_ptr<Expression>;
}
