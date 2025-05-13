#pragma once

#include <Compiler/API.h>
#include <Syntax/NodeKind.h>
#include <Semantic/Type.h>

namespace Caracal
{
    class COMPILER_API Node
    {
    public:
        Node(NodeKind kind, const Type& type);

        [[nodiscard]] NodeKind kind() const noexcept { return m_kind; }
        [[nodiscard]] Type type() const noexcept { return m_type; }

    private:
        NodeKind m_kind;
        Type m_type;
    };
}
