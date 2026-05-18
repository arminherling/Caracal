#pragma once

#include <Caracal/IR/ValueRef.h>

namespace Caracal
{
    class PhiInput
    {
    public:
        PhiInput(BlockId blockId, ValueRef value) noexcept;

        [[nodiscard]] BlockId blockId() const noexcept { return m_blockId; }
        [[nodiscard]] ValueRef value() const noexcept { return m_value; }

    private:
        BlockId m_blockId;
        ValueRef m_value;
    };
}
