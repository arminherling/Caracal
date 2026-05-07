#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Node.h>
#include <Caracal/Syntax/TokenBuffer.h>

namespace Caracal
{
    class CARACAL_API Expression : public Node
    {
    public:
        Expression(NodeKind kind, const Type& type);

        [[nodiscard]] bool isLiteral() const noexcept;
        [[nodiscard]] virtual SourceLocation sourceLocation(const TokenBuffer& tokens) const = 0;
    };

    using ExpressionUPtr = std::unique_ptr<Expression>;
}
