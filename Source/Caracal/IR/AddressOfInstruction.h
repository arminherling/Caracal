#pragma once

#include <Caracal/IR/Instruction.h>
#include <Caracal/IR/LocalSlotRef.h>
#include <Caracal/IR/ValueRef.h>

namespace Caracal
{
    class AddressOfInstruction final : public Instruction
    {
    public:
        AddressOfInstruction(TemporaryId resultId, LocalSlotRef local, Type type) noexcept;

        [[nodiscard]] TemporaryId resultId() const noexcept { return m_resultId; }
        [[nodiscard]] LocalSlotRef local() const noexcept { return m_local; }
        [[nodiscard]] Type type() const noexcept { return m_type; }
        void remapValueIds(const ValueIdMap& remap) noexcept override;

    private:
        TemporaryId m_resultId;
        LocalSlotRef m_local;
        Type m_type;
    };
}
