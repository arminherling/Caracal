#include <Caracal/Diagnostics/DiagnosticsBag.h>

namespace Caracal
{
    void DiagnosticsBag::AddError(DiagnosticKind kind, const SourceLocation& location)
    {
        diagnostics.emplace_back(DiagnosticLevel::Error, kind, location);
    }

    void DiagnosticsBag::AddIllegalCharacterError(
        const SourceTextSharedPtr& source,
        const SourceLocation& location)
    {
        diagnostics.emplace_back(
            DiagnosticLevel::Error,
            DiagnosticKind::_0001_FoundIllegalCharacter,
            source,
            location,
            "Remove the unsupported character.");
    }

    void DiagnosticsBag::AddUnterminatedStringError(
        const SourceTextSharedPtr& source,
        const SourceLocation& location)
    {
        diagnostics.emplace_back(
            DiagnosticLevel::Error,
            DiagnosticKind::_0002_UnterminatedString,
            source,
            location,
            "Add a closing quote to terminate the string.");
    }

    const std::vector<Diagnostic>& DiagnosticsBag::Diagnostics() const
    {
        return diagnostics;
    }
}