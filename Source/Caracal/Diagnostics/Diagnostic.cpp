#include <Caracal/Diagnostics/Diagnostic.h>

namespace Caracal
{
    namespace
    {
        std::string diagnosticCode(DiagnosticKind kind)
        {
            switch (kind)
            {
                case DiagnosticKind::_0001_IllegalCharacter:
                    return "C0001";
                case DiagnosticKind::_0002_UnterminatedString:
                    return "C0002";
                case DiagnosticKind::_0003_UnexpectedToken:
                    return "C0003";
                case DiagnosticKind::_0004_UnexpectedTrailingTokens:
                    return "C0004";
                case DiagnosticKind::_0005_InvalidEnumField:
                    return "C0005";
                case DiagnosticKind::_0006_UnexpectedAnnotation:
                    return "C0006";
                case DiagnosticKind::_0007_UnknownAnnotation:
                    return "C0007";
                case DiagnosticKind::_0008_MissingAnnotationArguments:
                    return "C0008";
                case DiagnosticKind::_0009_WrongNumberOfAnnotationArguments:
                    return "C0009";
                case DiagnosticKind::_0010_UnexpectedTopLevelToken:
                    return "C0010";
                case DiagnosticKind::_0011_InvalidStatement:
                    return "C0011";
                case DiagnosticKind::_0012_InvalidExpression:
                    return "C0012";
                case DiagnosticKind::Unknown:
                default:
                    return "C????";
            }
        }

        std::string diagnosticMessage(DiagnosticKind kind)
        {
            switch (kind)
            {
                case DiagnosticKind::_0001_IllegalCharacter:
                    return "Illegal character";
                case DiagnosticKind::_0002_UnterminatedString:
                    return "Unterminated string";
                case DiagnosticKind::_0003_UnexpectedToken:
                    return "Unexpected token";
                case DiagnosticKind::_0004_UnexpectedTrailingTokens:
                    return "Unexpected trailing tokens";
                case DiagnosticKind::_0005_InvalidEnumField:
                    return "Invalid enum field";
                case DiagnosticKind::_0006_UnexpectedAnnotation:
                    return "Unexpected annotation";
                case DiagnosticKind::_0007_UnknownAnnotation:
                    return "Unknown annotation";
                case DiagnosticKind::_0008_MissingAnnotationArguments:
                    return "Missing annotation arguments";
                case DiagnosticKind::_0009_WrongNumberOfAnnotationArguments:
                    return "Wrong number of annotation arguments";
                case DiagnosticKind::_0010_UnexpectedTopLevelToken:
                    return "Unexpected top-level token";
                case DiagnosticKind::_0011_InvalidStatement:
                    return "Invalid statement";
                case DiagnosticKind::_0012_InvalidExpression:
                    return "Invalid expression";
                case DiagnosticKind::Unknown:
                default:
                    return "Unknown diagnostic";
            }
        }
    }

    Diagnostic::Diagnostic(
        DiagnosticLevel level,
        DiagnosticKind kind,
        const SourceTextSharedPtr& source,
        const SourceLocation& location,
        std::optional<std::string> fix)
        : m_level(level)
        , m_kind(kind)
        , m_code(diagnosticCode(kind))
        , m_message(diagnosticMessage(kind))
        , m_source(source)
        , m_location(location)
        , m_fix(std::move(fix))
    {
    }

    void Diagnostic::addPrimaryLabel(const SourceLocation& location, std::string text)
    {
        m_labels.push_back(Label{
            .source = m_source,
            .location = location,
            .text = std::move(text),
            .isPrimary = true
            });
    }
}
