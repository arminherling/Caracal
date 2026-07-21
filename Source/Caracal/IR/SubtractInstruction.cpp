#include <Caracal/IR/SubtractInstruction.h>

namespace Caracal
{
    SubtractInstruction::SubtractInstruction(TemporaryId resultId, ValueRef leftValue, ValueRef rightValue, Type type) noexcept
        : Instruction{ InstructionKind::Subtract }
        , m_resultId{ resultId }
        , m_leftValue{ leftValue }
        , m_rightValue{ rightValue }
        , m_type{ type }
    {
    }

    void SubtractInstruction::remapValueIds(const ValueIdMap& remap) noexcept
    {
        m_resultId = remapTemporaryId(remap, m_resultId);
        m_leftValue = ValueRef{ remapTemporaryId(remap, m_leftValue.id()) };
        m_rightValue = ValueRef{ remapTemporaryId(remap, m_rightValue.id()) };
    }
}
