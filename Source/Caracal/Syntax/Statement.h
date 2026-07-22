#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Node.h>

namespace Caracal
{
    class CARACAL_API Statement : public Node
    {
    public:
        Statement(NodeKind kind, const Type& type);
        virtual ~Statement() = default;
    };

    using StatementUPtr = std::unique_ptr<Statement>;
}
