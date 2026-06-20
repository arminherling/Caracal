#include <Caracal/IR/LogicalAndInstruction.h>

namespace Caracal
{
    LogicalAndInstruction::LogicalAndInstruction(TemporaryId resultId, ValueRef leftValue, ValueRef rightValue, Type type) noexcept
        : Instruction{ InstructionKind::LogicalAnd }
        , m_resultId{ resultId }
        , m_leftValue{ leftValue }
        , m_rightValue{ rightValue }
        , m_type{ type }
    {
    }
}
