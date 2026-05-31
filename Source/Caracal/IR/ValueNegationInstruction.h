#pragma once

#include <Caracal/IR/Instruction.h>
#include <Caracal/IR/ValueRef.h>

namespace Caracal
{
    class ValueNegationInstruction final : public Instruction
    {
    public:
        ValueNegationInstruction(TemporaryId resultId, ValueRef operandValue, Type type) noexcept;

        [[nodiscard]] TemporaryId resultId() const noexcept { return m_resultId; }
        [[nodiscard]] ValueRef operandValue() const noexcept { return m_operandValue; }
        [[nodiscard]] Type type() const noexcept { return m_type; }

    private:
        TemporaryId m_resultId;
        ValueRef m_operandValue;
        Type m_type;
    };
}
