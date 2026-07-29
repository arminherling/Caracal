#include <Caracal/IR/SizeOfInstruction.h>

namespace Caracal
{
    SizeOfInstruction::SizeOfInstruction(TemporaryId resultId, Type measuredType, Type type) noexcept
        : Instruction{ InstructionKind::SizeOf }
        , m_resultId{ resultId }
        , m_measuredType{ measuredType }
        , m_type{ type }
    {
    }

    void SizeOfInstruction::remapValueIds(const ValueIdMap& remap) noexcept
    {
        m_resultId = remapTemporaryId(remap, m_resultId);
    }
}
