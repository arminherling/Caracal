#pragma once

#include <Caracal/API.h>
#include <Caracal/Diagnostics/Diagnostic.h>

#include <vector>

namespace Caracal
{
    class CARACAL_API DiagnosticsBag
    {
    public:
        DiagnosticsBag() = default;

        void AddError(DiagnosticKind kind, const SourceLocation& location);
        void AddIllegalCharacterError(const SourceTextSharedPtr& source, const SourceLocation& location);
        void AddUnterminatedStringError(const SourceTextSharedPtr& source, const SourceLocation& location);

        const std::vector<Diagnostic>& Diagnostics() const;

    private:
        std::vector<Diagnostic> diagnostics;
    };
}