#include <Caracal/IR/PhiInput.h>

namespace Caracal
{
    PhiInput::PhiInput(BlockId blockId, ValueRef value) noexcept
        : m_blockId{ blockId }
        , m_value{ value }
    {
    }
}
