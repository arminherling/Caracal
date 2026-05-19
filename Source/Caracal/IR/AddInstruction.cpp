#include <Caracal/IR/AddInstruction.h>

namespace Caracal
{
    AddInstruction::AddInstruction(TemporaryId resultId, ValueRef leftValue, ValueRef rightValue, Type type) noexcept
        : Instruction{ InstructionKind::Add }
        , m_resultId{ resultId }
        , m_leftValue{ leftValue }
        , m_rightValue{ rightValue }
        , m_type{ type }
    {
    }
}
