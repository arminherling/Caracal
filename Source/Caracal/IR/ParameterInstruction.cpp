#include <Caracal/IR/ParameterInstruction.h>

namespace Caracal
{
    ParameterInstruction::ParameterInstruction(TemporaryId resultId, i32 parameterIndex, Type type) noexcept
        : Instruction{ InstructionKind::Parameter }
        , m_resultId{ resultId }
        , m_parameterIndex{ parameterIndex }
        , m_type{ type }
    {
    }
}
