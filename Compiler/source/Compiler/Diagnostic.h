#pragma once

#include <Compiler/API.h>
#include <Compiler/DiagnosticLevel.h>
#include <Compiler/DiagnosticKind.h>
#include <Text/SourceLocation.h>

namespace Caracal
{
    struct COMPILER_API Diagnostic
    {
        DiagnosticLevel level = DiagnosticLevel::Unknown;
        DiagnosticKind kind = DiagnosticKind::Unknown;
        SourceLocation location;
    };
}
