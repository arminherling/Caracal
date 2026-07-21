#include <Caracal/IR/Instruction.h>

namespace Caracal
{
    Instruction::Instruction(InstructionKind kind) noexcept
        : m_kind{ kind }
    {
    }

    TemporaryId remapTemporaryId(const ValueIdMap& remap, TemporaryId id) noexcept
    {
        const auto foundId = remap.find(id);
        if (foundId != remap.end())
        {
            return foundId->second;
        }

        return id;
    }
}
