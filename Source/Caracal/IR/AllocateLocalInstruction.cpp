#include <Caracal/IR/AllocateLocalInstruction.h>

namespace Caracal
{
    AllocateLocalInstruction::AllocateLocalInstruction(LocalSlotId resultId, std::string localName, Type type) noexcept
        : Instruction{ InstructionKind::AllocateLocal }
        , m_resultId{ resultId }
        , m_localName{ std::move(localName) }
        , m_type{ type }
    {
    }
}
