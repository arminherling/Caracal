#include <Caracal/IR/Instruction.h>

namespace Caracal
{
    Instruction::Instruction(InstructionKind kind) noexcept
        : m_kind{ kind }
    {
    }
}
