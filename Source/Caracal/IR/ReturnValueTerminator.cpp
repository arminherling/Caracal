#include <Caracal/IR/ReturnValueTerminator.h>

namespace Caracal
{
    ReturnValueTerminator::ReturnValueTerminator(ValueRef value) noexcept
        : Terminator{ TerminatorKind::ReturnValue }
        , m_value{ value }
    {
    }

    void ReturnValueTerminator::remapValueIds(const ValueIdMap& remap) noexcept
    {
        m_value = ValueRef{ remapTemporaryId(remap, m_value.id()) };
    }
}
