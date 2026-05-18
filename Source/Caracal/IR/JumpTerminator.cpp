#include <Caracal/IR/JumpTerminator.h>

namespace Caracal
{
    JumpTerminator::JumpTerminator(BlockId targetBlockId) noexcept
        : Terminator{ TerminatorKind::Jump }
        , m_targetBlockId{ targetBlockId }
    {
    }
}
