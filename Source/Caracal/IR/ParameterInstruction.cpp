#include <Caracal/IR/ParameterInstruction.h>

namespace Caracal
{
    ParameterInstruction::ParameterInstruction(TemporaryId resultId, i32 parameterIndex, IRParameter parameter) noexcept
        : Instruction{ InstructionKind::Parameter }
        , m_resultId{ resultId }
        , m_parameterIndex{ parameterIndex }
        , m_parameter{ std::move(parameter) }
    {
    }

    void ParameterInstruction::remapValueIds(const ValueIdMap& remap) noexcept
    {
        m_resultId = remapTemporaryId(remap, m_resultId);
    }
}
