#pragma once

#include <Caracal/API.h>
#include <Caracal/Diagnostics/Diagnostic.h>
#include <Caracal/Diagnostics/DiagnosticOptions.h>
#include <Caracal/Diagnostics/DiagnosticsBag.h>
#include <Caracal/Text/SourceText.h>

#include <iosfwd>
#include <string>

namespace Caracal
{
    CARACAL_API void writeDiagnostic(
        std::ostream& outStream,
        const Diagnostic& diagnostic,
        const DiagnosticOptions& options = {});

    CARACAL_API void writeDiagnostics(
        std::ostream& outStream,
        DiagnosticsBag& diagnostics,
        const DiagnosticOptions& options = {});

    CARACAL_API std::string formatDiagnostic(
        const Diagnostic& diagnostic,
        const DiagnosticOptions& options = {});

    CARACAL_API std::string formatDiagnostics(
        DiagnosticsBag& diagnostics,
        const DiagnosticOptions& options = {});
}
