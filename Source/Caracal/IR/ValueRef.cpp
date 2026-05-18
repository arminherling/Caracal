#include <Caracal/IR/ValueRef.h>

namespace Caracal
{
    ValueRef::ValueRef(TemporaryId id) noexcept
        : m_id{ id }
    {
    }
}
