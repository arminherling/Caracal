#pragma once

#include <Caracal/IR/Instruction.h>
#include <Caracal/IR/PhiInput.h>

#include <utility>
#include <vector>

namespace Caracal
{
    class PhiInstruction final : public Instruction
    {
    public:
        PhiInstruction(TemporaryId resultId, std::vector<PhiInput> inputs, Type type) noexcept;

        [[nodiscard]] TemporaryId resultId() const noexcept { return m_resultId; }
        [[nodiscard]] const std::vector<PhiInput>& inputs() const noexcept { return m_inputs; }
        [[nodiscard]] Type type() const noexcept { return m_type; }

    private:
        TemporaryId m_resultId;
        std::vector<PhiInput> m_inputs;
        Type m_type;
    };
}
