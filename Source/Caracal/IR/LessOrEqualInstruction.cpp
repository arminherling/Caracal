#include <Caracal/IR/LessOrEqualInstruction.h>

namespace Caracal
{
    LessOrEqualInstruction::LessOrEqualInstruction(TemporaryId resultId, ValueRef leftValue, ValueRef rightValue, Type type) noexcept
        : Instruction{ InstructionKind::LessOrEqual }
        , m_resultId{ resultId }
        , m_leftValue{ leftValue }
        , m_rightValue{ rightValue }
        , m_type{ type }
    {
    }
}
