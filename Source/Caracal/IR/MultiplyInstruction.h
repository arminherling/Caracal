#pragma once

#include <Caracal/IR/Instruction.h>
#include <Caracal/IR/ValueRef.h>

namespace Caracal
{
    class MultiplyInstruction final : public Instruction
    {
    public:
        MultiplyInstruction(TemporaryId resultId, ValueRef leftValue, ValueRef rightValue, Type type) noexcept;

        [[nodiscard]] TemporaryId resultId() const noexcept { return m_resultId; }
        [[nodiscard]] ValueRef leftValue() const noexcept { return m_leftValue; }
        [[nodiscard]] ValueRef rightValue() const noexcept { return m_rightValue; }
        [[nodiscard]] Type type() const noexcept { return m_type; }
        void remapValueIds(const ValueIdMap& remap) noexcept override;

    private:
        TemporaryId m_resultId;
        ValueRef m_leftValue;
        ValueRef m_rightValue;
        Type m_type;
    };
}
