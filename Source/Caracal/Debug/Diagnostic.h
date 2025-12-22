#pragma once

#include <Caracal/API.h>
#include <Caracal/Debug/DiagnosticLevel.h>
#include <Caracal/Debug/DiagnosticKind.h>
#include <Caracal/Text/SourceLocation.h>

namespace Caracal
{
    struct CARACAL_API Diagnostic
    {
        DiagnosticLevel level = DiagnosticLevel::Unknown;
        DiagnosticKind kind = DiagnosticKind::Unknown;
        SourceLocation location;
    };
}
