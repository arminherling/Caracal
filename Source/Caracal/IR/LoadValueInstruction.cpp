#include <Caracal/IR/LoadValueInstruction.h>

namespace Caracal
{
    LoadValueInstruction::LoadValueInstruction(TemporaryId resultId, ValueRef address, Type type) noexcept
        : Instruction{ InstructionKind::LoadValue }
        , m_resultId{ resultId }
        , m_address{ address }
        , m_type{ type }
    {
    }

    void LoadValueInstruction::remapValueIds(const ValueIdMap& remap) noexcept
    {
        m_resultId = remapTemporaryId(remap, m_resultId);
        m_address = ValueRef{ remapTemporaryId(remap, m_address.id()) };
    }
}
