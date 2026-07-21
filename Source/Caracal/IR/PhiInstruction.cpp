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

    void PhiInstruction::setInputs(std::vector<PhiInput> inputs) noexcept
    {
        m_inputs = std::move(inputs);
    }

    void PhiInstruction::remapValueIds(const ValueIdMap& remap) noexcept
    {
        m_resultId = remapTemporaryId(remap, m_resultId);
        for (auto& input : m_inputs)
        {
            input = PhiInput{ input.blockId(), ValueRef{ remapTemporaryId(remap, input.value().id()) } };
        }
    }
}
