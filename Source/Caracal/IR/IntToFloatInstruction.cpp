#include <Caracal/IR/IntToFloatInstruction.h>

namespace Caracal
{
    IntToFloatInstruction::IntToFloatInstruction(TemporaryId resultId, ValueRef operandValue, Type sourceType, Type type) noexcept
        : Instruction{ InstructionKind::IntToFloat }
        , m_resultId{ resultId }
        , m_operandValue{ operandValue }
        , m_sourceType{ sourceType }
        , m_type{ type }
    {
    }

    void IntToFloatInstruction::remapValueIds(const ValueIdMap& remap) noexcept
    {
        m_resultId = remapTemporaryId(remap, m_resultId);
        m_operandValue = ValueRef{ remapTemporaryId(remap, m_operandValue.id()) };
    }
}
