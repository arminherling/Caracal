#pragma once

#include <Caracal/API.h>
#include <Caracal/IR/Module.h>

namespace Caracal
{
    CARACAL_API void eliminateDeadCode(Module& module) noexcept;
}
