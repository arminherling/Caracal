#include <Caracal/IR/Terminator.h>

namespace Caracal
{
    Terminator::Terminator(TerminatorKind kind) noexcept
        : m_kind{ kind }
    {
    }
}
