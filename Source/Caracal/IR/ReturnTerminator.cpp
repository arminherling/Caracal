#include <Caracal/IR/ReturnTerminator.h>

namespace Caracal
{
    ReturnTerminator::ReturnTerminator() noexcept
        : Terminator{ TerminatorKind::Return }
    {
    }
}
