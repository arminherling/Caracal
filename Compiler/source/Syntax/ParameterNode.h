#pragma once

#include <Compiler/API.h>
#include <Syntax/Node.h>
#include <memory>

namespace Caracal
{
    class COMPILER_API ParameterNode : public Node
    {
    public:
        ParameterNode();

    private:

    };

    using ParameterNodeUPtr = std::unique_ptr<ParameterNode>;
}
