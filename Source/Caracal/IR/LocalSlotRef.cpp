#include <Caracal/IR/LocalSlotRef.h>

namespace Caracal
{
    LocalSlotRef::LocalSlotRef()
        : m_id{ -1 }
    {
    }

    LocalSlotRef::LocalSlotRef(LocalSlotId id) noexcept
        : m_id{ id }
    {
    }
}
