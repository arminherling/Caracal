#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/NodeKind.h>
#include <Caracal/Semantic/Type.h>
#include <memory>
#include <optional>

namespace Caracal
{
    class CARACAL_API Node
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
