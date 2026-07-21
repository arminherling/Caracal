#include <Caracal/IR/BranchIfTerminator.h>

namespace Caracal
{
    BranchIfTerminator::BranchIfTerminator(ValueRef condition, BlockId trueBlockId, BlockId falseBlockId) noexcept
        : Terminator{ TerminatorKind::Branch }
        , m_condition{ condition }
        , m_trueBlockId{ trueBlockId }
        , m_falseBlockId{ falseBlockId }
    {
    }

    void BranchIfTerminator::remapValueIds(const ValueIdMap& remap) noexcept
    {
        m_condition = ValueRef{ remapTemporaryId(remap, m_condition.id()) };
    }
}
