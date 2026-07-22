#include "Type.h"

namespace Caracal
{
    Type::Type(i32 id, TypeKind kind)
        : m_id{ id }
        , m_kind{ kind }
    {
    }

    bool operator==(Type lhs, Type rhs) noexcept
    {
        return lhs.id() == rhs.id() && lhs.kind() == rhs.kind();
    }

    bool operator!=(Type lhs, Type rhs) noexcept
    {
        return !(lhs == rhs);
    }
}
