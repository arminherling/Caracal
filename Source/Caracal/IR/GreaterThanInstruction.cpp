#include <Caracal/IR/GreaterThanInstruction.h>

namespace Caracal
{
    GreaterThanInstruction::GreaterThanInstruction(TemporaryId resultId, ValueRef leftValue, ValueRef rightValue, Type type) noexcept
        : Instruction{ InstructionKind::GreaterThan }
        , m_resultId{ resultId }
        , m_leftValue{ leftValue }
        , m_rightValue{ rightValue }
        , m_type{ type }
    {
    }
}
