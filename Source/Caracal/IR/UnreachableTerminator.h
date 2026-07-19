#pragma once

#include <Caracal/IR/Terminator.h>

namespace Caracal
{
    class UnreachableTerminator final : public Terminator
    {
    public:
        UnreachableTerminator() noexcept;
    };
}
