#include <Caracal/IR/StoreValueInstruction.h>

namespace Caracal
{
    StoreValueInstruction::StoreValueInstruction(ValueRef value, ValueRef address, Type type) noexcept
        : Instruction{ InstructionKind::StoreValue }
        , m_value{ value }
        , m_address{ address }
        , m_type{ type }
    {
    }

    void StoreValueInstruction::remapValueIds(const ValueIdMap& remap) noexcept
    {
        m_value = ValueRef{ remapTemporaryId(remap, m_value.id()) };
        m_address = ValueRef{ remapTemporaryId(remap, m_address.id()) };
    }
}
