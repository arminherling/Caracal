#pragma once

#include <Compiler/API.h>
#include <Syntax/Node.h>

namespace Caracal
{
    class COMPILER_API Expression : public Node
    {
    public:
        Expression(NodeKind kind, const Type& type);
    };
}
