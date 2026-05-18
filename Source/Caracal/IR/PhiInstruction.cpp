#include <Caracal/IR/PhiInstruction.h>

namespace Caracal
{
    PhiInstruction::PhiInstruction(TemporaryId resultId, std::vector<PhiInput> inputs, Type type) noexcept
        : Instruction{ InstructionKind::Phi }
        , m_resultId{ resultId }
        , m_inputs{ std::move(inputs) }
        , m_type{ type }
    {
    }
}
