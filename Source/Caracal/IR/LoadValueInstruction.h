#pragma once

#include <Caracal/IR/Instruction.h>
#include <Caracal/IR/ValueRef.h>

namespace Caracal
{
    class LoadValueInstruction final : public Instruction
    {
    public:
        LoadValueInstruction(TemporaryId resultId, ValueRef address, Type type) noexcept;

        [[nodiscard]] TemporaryId resultId() const noexcept { return m_resultId; }
        [[nodiscard]] ValueRef address() const noexcept { return m_address; }
        [[nodiscard]] Type type() const noexcept { return m_type; }

    private:
        TemporaryId m_resultId;
        ValueRef m_address;
        Type m_type;
    };
}
