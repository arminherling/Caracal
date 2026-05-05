#include <Caracal/Diagnostics/Diagnostic.h>

namespace Caracal
{
    namespace
    {
        std::string diagnosticCode(DiagnosticKind kind)
        {
            switch (kind)
            {
            case DiagnosticKind::_0001_FoundIllegalCharacter:
                return "C0001";
            case DiagnosticKind::_0002_UnterminatedString:
                return "C0002";
            case DiagnosticKind::_0003_ExpectedXButGotY:
                return "C0003";
            case DiagnosticKind::_0004_ExtraTokensRemaining:
                return "C0004";
            case DiagnosticKind::_0005_ExpectedEnumField:
                return "C0005";
            case DiagnosticKind::_0006_UnexpectedAnnotation:
                return "C0006";
            case DiagnosticKind::_0007_UnknownAnnotation:
                return "C0007";
            case DiagnosticKind::_0008_AnnotationMissingArguments:
                return "C0008";
            case DiagnosticKind::_0009_AnnotationWrongNumberOfArguments:
                return "C0009";
            case DiagnosticKind::Unknown:
            default:
                return "C0000";
            }
        }

        std::string diagnosticMessage(DiagnosticKind kind)
        {
            switch (kind)
            {
            case DiagnosticKind::_0001_FoundIllegalCharacter:
                return "Illegal character";
            case DiagnosticKind::_0002_UnterminatedString:
                return "Unterminated string";
            case DiagnosticKind::_0003_ExpectedXButGotY:
                return "Expected a different token";
            case DiagnosticKind::_0004_ExtraTokensRemaining:
                return "Extra tokens remaining";
            case DiagnosticKind::_0005_ExpectedEnumField:
                return "Expected enum field";
            case DiagnosticKind::_0006_UnexpectedAnnotation:
                return "Unexpected annotation";
            case DiagnosticKind::_0007_UnknownAnnotation:
                return "Unknown annotation";
            case DiagnosticKind::_0008_AnnotationMissingArguments:
                return "Annotation missing arguments";
            case DiagnosticKind::_0009_AnnotationWrongNumberOfArguments:
                return "Annotation has wrong number of arguments";
            case DiagnosticKind::Unknown:
            default:
                return "Unknown diagnostic";
            }
        }
    }

    Diagnostic::Diagnostic(
        DiagnosticLevel level,
        DiagnosticKind kind,
        const SourceLocation& location,
        std::optional<std::string> fix)
        : Diagnostic(level, kind, SourceTextSharedPtr{}, location, std::move(fix))
    {
    }

    Diagnostic::Diagnostic(
        DiagnosticLevel level,
        DiagnosticKind kind,
        const SourceTextSharedPtr& source,
        const SourceLocation& location,
        std::optional<std::string> fix)
        : level(level)
        , kind(kind)
        , code(diagnosticCode(kind))
        , message(diagnosticMessage(kind))
        , source(source)
        , location(location)
        , fix(std::move(fix))
    {
    }
}