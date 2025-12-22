#pragma once

#include <Caracal/Defines.h>
#include <Caracal/API.h>

namespace Caracal
{
    struct CARACAL_API SourceLocation
    {
        i32 startIndex = -1;
        i32 endIndex = -1;
    };
}
