#include <Caracal/IR/AddressOfGlobalInstruction.h>

#include <utility>

namespace Caracal
{
    AddressOfGlobalInstruction::AddressOfGlobalInstruction(TemporaryId resultId, std::string name, Type type) noexcept
        : Instruction{ InstructionKind::AddressOfGlobal }
        , m_resultId{ resultId }
        , m_name{ std::move(name) }
        , m_type{ type }
    {
    }

    void AddressOfGlobalInstruction::remapValueIds(const ValueIdMap& remap) noexcept
    {
        m_resultId = remapTemporaryId(remap, m_resultId);
    }
}
