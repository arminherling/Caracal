#pragma once

#include <Caracal/IR/ConstantValue.h>
#include <Caracal/IR/Instruction.h>

namespace Caracal
{
    class ConstantInstruction final : public Instruction
    {
    public:
        ConstantInstruction(TemporaryId resultId, ConstantValue value, Type type) noexcept;

        [[nodiscard]] TemporaryId resultId() const noexcept { return m_resultId; }
        [[nodiscard]] const ConstantValue& value() const noexcept { return m_value; }
        [[nodiscard]] Type type() const noexcept { return m_type; }

    private:
        TemporaryId m_resultId;
        ConstantValue m_value;
        Type m_type;
    };
}
