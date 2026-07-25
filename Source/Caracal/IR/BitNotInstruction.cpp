#include <Caracal/IR/BitNotInstruction.h>

namespace Caracal
{
    BitNotInstruction::BitNotInstruction(TemporaryId resultId, ValueRef operandValue, Type type) noexcept
        : Instruction{ InstructionKind::BitNot }
        , m_resultId{ resultId }
        , m_operandValue{ operandValue }
        , m_type{ type }
    {
    }

    void BitNotInstruction::remapValueIds(const ValueIdMap& remap) noexcept
    {
        m_resultId = remapTemporaryId(remap, m_resultId);
        m_operandValue = ValueRef{ remapTemporaryId(remap, m_operandValue.id()) };
    }
}
