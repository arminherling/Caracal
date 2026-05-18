#include <Caracal/IR/BranchTerminator.h>

namespace Caracal
{
    BranchTerminator::BranchTerminator(ValueRef condition, BlockId trueBlockId, BlockId falseBlockId) noexcept
        : Terminator{ TerminatorKind::Branch }
        , m_condition{ condition }
        , m_trueBlockId{ trueBlockId }
        , m_falseBlockId{ falseBlockId }
    {
    }
}
