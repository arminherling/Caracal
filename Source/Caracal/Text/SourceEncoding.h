#pragma once

#include <Caracal/API.h>
#include <Caracal/Diagnostics/DiagnosticsBag.h>

namespace Caracal
{
    // source files must be UTF-8, other encodings are rejected before lexing starts
    [[nodiscard]] CARACAL_API bool validateSourceEncoding(const SourceTextSharedPtr& sourceText, DiagnosticsBag& diagnostics) noexcept;
}
