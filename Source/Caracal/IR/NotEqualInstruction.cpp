#include <Caracal/IR/NotEqualInstruction.h>

namespace Caracal
{
    NotEqualInstruction::NotEqualInstruction(TemporaryId resultId, ValueRef leftValue, ValueRef rightValue, Type type) noexcept
        : Instruction{ InstructionKind::NotEqual }
        , m_resultId{ resultId }
        , m_leftValue{ leftValue }
        , m_rightValue{ rightValue }
        , m_type{ type }
    {
    }
}
