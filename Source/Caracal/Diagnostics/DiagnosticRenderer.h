#pragma once

#include <Caracal/API.h>
#include <Caracal/Diagnostics/Diagnostic.h>
#include <Caracal/Diagnostics/DiagnosticsBag.h>
#include <Caracal/Text/SourceText.h>

#include <iosfwd>
#include <string>

namespace Caracal
{
    CARACAL_API void writeDiagnostic(
        std::ostream& outStream,
        const Diagnostic& diagnostic,
        const SourceText& fallbackSource,
        bool enableColors = true,
        bool enableUnicode = true);

    CARACAL_API void writeDiagnostics(
        std::ostream& outStream,
        DiagnosticsBag& diagnostics,
        const SourceText& fallbackSource,
        bool enableColors = true,
        bool enableUnicode = true);

    CARACAL_API std::string renderDiagnostic(
        const Diagnostic& diagnostic,
        const SourceText& source,
        bool enableColors = true,
        bool enableUnicode = true);

    CARACAL_API std::string renderDiagnostics(
        DiagnosticsBag& diagnostics,
        const SourceText& source,
        bool enableColors = true,
        bool enableUnicode = true);
}