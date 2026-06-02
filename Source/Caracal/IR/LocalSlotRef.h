#pragma once

#include <Caracal/IR/Instruction.h>

namespace Caracal
{
    class LocalSlotRef
    {
    public:
        LocalSlotRef();
        explicit LocalSlotRef(LocalSlotId id) noexcept;

        [[nodiscard]] LocalSlotId id() const noexcept { return m_id; }

    private:
        LocalSlotId m_id;
    };
}
