#pragma once

#include <Caracal/IR/Instruction.h>
#include <Caracal/IR/ValueRef.h>

namespace Caracal
{
    class LogicalNegationInstruction final : public Instruction
    {
    public:
        LogicalNegationInstruction(TemporaryId resultId, ValueRef operandValue, Type type) noexcept;

        [[nodiscard]] TemporaryId resultId() const noexcept { return m_resultId; }
        [[nodiscard]] ValueRef operandValue() const noexcept { return m_operandValue; }
        [[nodiscard]] Type type() const noexcept { return m_type; }

    private:
        TemporaryId m_resultId;
        ValueRef m_operandValue;
        Type m_type;
    };
}
