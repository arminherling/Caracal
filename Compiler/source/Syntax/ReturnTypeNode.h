#pragma once

#include <Compiler/API.h>
#include <Syntax/Node.h>

namespace Caracal
{
    class COMPILER_API ReturnTypeNode : public Node
    {
    public:
        ReturnTypeNode();

    private:

    };

    using ReturnTypeNodeUPtr = std::unique_ptr<ReturnTypeNode>;
}
