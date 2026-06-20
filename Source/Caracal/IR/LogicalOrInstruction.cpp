#include <Caracal/IR/LogicalOrInstruction.h>

namespace Caracal
{
    LogicalOrInstruction::LogicalOrInstruction(TemporaryId resultId, ValueRef leftValue, ValueRef rightValue, Type type) noexcept
        : Instruction{ InstructionKind::LogicalOr }
        , m_resultId{ resultId }
        , m_leftValue{ leftValue }
        , m_rightValue{ rightValue }
        , m_type{ type }
    {
    }
}
