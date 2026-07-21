#include <Caracal/IR/ValueNegationInstruction.h>

namespace Caracal
{
    ValueNegationInstruction::ValueNegationInstruction(TemporaryId resultId, ValueRef operandValue, Type type) noexcept
        : Instruction{ InstructionKind::ValueNegation }
        , m_resultId{ resultId }
        , m_operandValue{ operandValue }
        , m_type{ type }
    {
    }

    void ValueNegationInstruction::remapValueIds(const ValueIdMap& remap) noexcept
    {
        m_resultId = remapTemporaryId(remap, m_resultId);
        m_operandValue = ValueRef{ remapTemporaryId(remap, m_operandValue.id()) };
    }
}
