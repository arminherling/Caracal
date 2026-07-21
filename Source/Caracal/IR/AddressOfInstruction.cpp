#include <Caracal/IR/AddressOfInstruction.h>

namespace Caracal
{
    AddressOfInstruction::AddressOfInstruction(TemporaryId resultId, LocalSlotRef local, Type type) noexcept
        : Instruction{ InstructionKind::AddressOf }
        , m_resultId{ resultId }
        , m_local{ local }
        , m_type{ type }
    {
    }

    void AddressOfInstruction::remapValueIds(const ValueIdMap& remap) noexcept
    {
        m_resultId = remapTemporaryId(remap, m_resultId);
    }
}
