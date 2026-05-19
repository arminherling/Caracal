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
}
