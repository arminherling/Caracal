#pragma once

#include <Caracal/IR/Instruction.h>

#include <string>

namespace Caracal
{
    class AllocateLocalInstruction final : public Instruction
    {
    public:
        AllocateLocalInstruction(LocalSlotId resultId, std::string localName, Type type) noexcept;

        [[nodiscard]] LocalSlotId resultId() const noexcept { return m_resultId; }
        [[nodiscard]] const std::string& localName() const noexcept { return m_localName; }
        [[nodiscard]] Type type() const noexcept { return m_type; }

    private:
        LocalSlotId m_resultId;
        std::string m_localName;
        Type m_type;
    };
}
