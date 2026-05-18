#pragma once

#include <Caracal/IR/Instruction.h>

namespace Caracal
{
    class ValueRef
    {
    public:
        explicit ValueRef(TemporaryId id) noexcept;

        [[nodiscard]] TemporaryId id() const noexcept { return m_id; }

    private:
        TemporaryId m_id;
    };
}
