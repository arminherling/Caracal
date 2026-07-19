#include <Caracal/IR/UnreachableTerminator.h>

namespace Caracal
{
    UnreachableTerminator::UnreachableTerminator() noexcept
        : Terminator{ TerminatorKind::Unreachable }
    {
    }
}
