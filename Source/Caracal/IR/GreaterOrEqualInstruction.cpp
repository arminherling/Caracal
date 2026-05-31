#include <Caracal/IR/GreaterOrEqualInstruction.h>

namespace Caracal
{
    GreaterOrEqualInstruction::GreaterOrEqualInstruction(TemporaryId resultId, ValueRef leftValue, ValueRef rightValue, Type type) noexcept
        : Instruction{ InstructionKind::GreaterOrEqual }
        , m_resultId{ resultId }
        , m_leftValue{ leftValue }
        , m_rightValue{ rightValue }
        , m_type{ type }
    {
    }
}
