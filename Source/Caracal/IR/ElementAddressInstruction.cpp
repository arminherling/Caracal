#include <Caracal/IR/ElementAddressInstruction.h>

namespace Caracal
{
    ElementAddressInstruction::ElementAddressInstruction(
        TemporaryId resultId,
        ValueRef baseAddress,
        Type arrayType,
        ValueRef index,
        Type type) noexcept
        : Instruction{ InstructionKind::ElementAddress }
        , m_resultId{ resultId }
        , m_baseAddress{ baseAddress }
        , m_arrayType{ arrayType }
        , m_index{ index }
        , m_type{ type }
    {
    }

    void ElementAddressInstruction::remapValueIds(const ValueIdMap& remap) noexcept
    {
        m_resultId = remapTemporaryId(remap, m_resultId);
        m_baseAddress = ValueRef{ remapTemporaryId(remap, m_baseAddress.id()) };
        m_index = ValueRef{ remapTemporaryId(remap, m_index.id()) };
    }
}
