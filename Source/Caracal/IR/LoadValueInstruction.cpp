#include <Caracal/IR/LoadValueInstruction.h>

namespace Caracal
{
    LoadValueInstruction::LoadValueInstruction(TemporaryId resultId, ValueRef address, Type type) noexcept
        : Instruction{ InstructionKind::LoadValue }
        , m_resultId{ resultId }
        , m_address{ address }
        , m_type{ type }
    {
    }
}
