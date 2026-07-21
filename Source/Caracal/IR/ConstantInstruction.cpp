#include <Caracal/IR/ConstantInstruction.h>

namespace Caracal
{
    ConstantInstruction::ConstantInstruction(TemporaryId resultId, ConstantValue value, Type type) noexcept
        : Instruction{ InstructionKind::Constant }
        , m_resultId{ resultId }
        , m_value{ value }
        , m_type{ type }
    {
    }

    void ConstantInstruction::remapValueIds(const ValueIdMap& remap) noexcept
    {
        m_resultId = remapTemporaryId(remap, m_resultId);
    }
}
