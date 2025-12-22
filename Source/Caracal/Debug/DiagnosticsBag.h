#pragma once

#include <Caracal/API.h>
#include <Caracal/Debug/Diagnostic.h>
#include <vector>

namespace Caracal
{
    class CARACAL_API DiagnosticsBag
    {
    public:
        DiagnosticsBag() = default;

        void AddWarning(DiagnosticKind kind, const SourceLocation& location);
        void AddError(DiagnosticKind kind, const SourceLocation& location);

        const std::vector<Diagnostic>& Diagnostics();

    private:
        std::vector<Diagnostic> diagnostics;
    };
}
