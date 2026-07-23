#include <Caracal/IR/MakeSliceInstruction.h>

namespace Caracal
{
    MakeSliceInstruction::MakeSliceInstruction(
        TemporaryId resultId,
        ValueRef baseAddress,
        ValueRef length,
        Type type) noexcept
        : Instruction{ InstructionKind::MakeSlice }
        , m_resultId{ resultId }
        , m_baseAddress{ baseAddress }
        , m_length{ length }
        , m_type{ type }
    {
    }

    void MakeSliceInstruction::remapValueIds(const ValueIdMap& remap) noexcept
    {
        m_resultId = remapTemporaryId(remap, m_resultId);
        m_baseAddress = ValueRef{ remapTemporaryId(remap, m_baseAddress.id()) };
        m_length = ValueRef{ remapTemporaryId(remap, m_length.id()) };
    }
}
