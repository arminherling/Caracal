#include <Caracal/IR/ShiftRightInstruction.h>

namespace Caracal
{
    ShiftRightInstruction::ShiftRightInstruction(TemporaryId resultId, ValueRef value, ValueRef amount, Type type) noexcept
        : Instruction{ InstructionKind::ShiftRight }
        , m_resultId{ resultId }
        , m_value{ value }
        , m_amount{ amount }
        , m_type{ type }
    {
    }

    void ShiftRightInstruction::remapValueIds(const ValueIdMap& remap) noexcept
    {
        m_resultId = remapTemporaryId(remap, m_resultId);
        m_value = ValueRef{ remapTemporaryId(remap, m_value.id()) };
        m_amount = ValueRef{ remapTemporaryId(remap, m_amount.id()) };
    }
}
