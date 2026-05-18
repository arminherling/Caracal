#pragma once

#include <Caracal/IR/Terminator.h>

namespace Caracal
{
    class JumpTerminator final : public Terminator
    {
    public:
        explicit JumpTerminator(BlockId targetBlockId) noexcept;

        [[nodiscard]] BlockId targetBlockId() const noexcept { return m_targetBlockId; }

    private:
        BlockId m_targetBlockId;
    };
}
