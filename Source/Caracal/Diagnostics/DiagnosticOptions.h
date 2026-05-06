#pragma once

#include <Caracal/API.h>

namespace Caracal
{
    struct CARACAL_API DiagnosticOptions
    {
        int contextLines = 3;
        bool enableColors = true;
        bool enableUnicode = true;
    };
}
