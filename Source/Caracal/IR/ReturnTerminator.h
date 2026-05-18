#pragma once

#include <Caracal/IR/Terminator.h>

namespace Caracal
{
    class ReturnTerminator final : public Terminator
    {
    public:
        ReturnTerminator() noexcept;
    };
}
