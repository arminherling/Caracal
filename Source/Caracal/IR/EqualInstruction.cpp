#include <Caracal/IR/EqualInstruction.h>

namespace Caracal
{
    EqualInstruction::EqualInstruction(TemporaryId resultId, ValueRef leftValue, ValueRef rightValue, Type type, Type operandType) noexcept
        : Instruction{ InstructionKind::Equal }
        , m_resultId{ resultId }
        , m_leftValue{ leftValue }
        , m_rightValue{ rightValue }
        , m_type{ type }
        , m_operandType{ operandType }
    {
    }
}
