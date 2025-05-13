#pragma once

#include <Defines.h>
#include <Compiler/API.h>

namespace Caracal
{
    struct COMPILER_API SourceLocation
    {
        i32 startIndex = -1;
        i32 endIndex = -1;
    };
}
