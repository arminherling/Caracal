#include <Caracal/IR/LogicalNegationInstruction.h>

namespace Caracal
{
    LogicalNegationInstruction::LogicalNegationInstruction(TemporaryId resultId, ValueRef operandValue, Type type) noexcept
        : Instruction{ InstructionKind::LogicalNegation }
        , m_resultId{ resultId }
        , m_operandValue{ operandValue }
        , m_type{ type }
    {
    }

    void LogicalNegationInstruction::remapValueIds(const ValueIdMap& remap) noexcept
    {
        m_resultId = remapTemporaryId(remap, m_resultId);
        m_operandValue = ValueRef{ remapTemporaryId(remap, m_operandValue.id()) };
    }
}
