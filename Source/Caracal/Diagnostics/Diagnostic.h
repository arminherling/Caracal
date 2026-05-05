#pragma once

#include <Caracal/API.h>
#include <Caracal/Debug/DiagnosticLevel.h>
#include <Caracal/Debug/DiagnosticKind.h>
#include <Caracal/Text/SourceLocation.h>
#include <Caracal/Text/SourceText.h>

#include <optional>
#include <string>
#include <vector>

namespace Caracal
{
    struct CARACAL_API Diagnostic
    {
        struct Label
        {
            SourceTextSharedPtr source;
            SourceLocation location;
            std::string text;
            bool isPrimary = false;
        };

        DiagnosticLevel level = DiagnosticLevel::Unknown;
        DiagnosticKind kind = DiagnosticKind::Unknown;
        std::string code;
        std::string message;
        SourceTextSharedPtr source;
        SourceLocation location;
        std::vector<Label> labels;
        std::optional<std::string> fix;

        Diagnostic() = default;
        Diagnostic(
            DiagnosticLevel level,
            DiagnosticKind kind,
            const SourceLocation& location,
            std::optional<std::string> fix = std::nullopt);
        Diagnostic(
            DiagnosticLevel level,
            DiagnosticKind kind,
            const SourceTextSharedPtr& source,
            const SourceLocation& location,
            std::optional<std::string> fix = std::nullopt);
    };
}