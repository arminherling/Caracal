#include <Caracal/IR/ValueRef.h>

namespace Caracal
{
    ValueRef::ValueRef()
        : m_id{ -1 }
    {
    }

    ValueRef::ValueRef(TemporaryId id) noexcept
        : m_id{ id }
    {
    }
}
