#include <Caracal/Diagnostics/DiagnosticRenderer.h>

#include <CaraReport.h>

#include <sstream>

namespace Caracal
{
    namespace
    {
        CaraReport::Level toCaraReportLevel(DiagnosticLevel level)
        {
            switch (level)
            {
            case DiagnosticLevel::Warning:
                return CaraReport::Level::Warning;
            case DiagnosticLevel::Error:
                return CaraReport::Level::Error;
            case DiagnosticLevel::Unknown:
            default:
                return CaraReport::Level::Info;
            }
        }

        CaraReport::Source createSource(const SourceText& source)
        {
            if (source.filePath.empty())
            {
                return CaraReport::Source("<input>", source.text);
            }

            return CaraReport::Source(source.filePath.generic_string(), source.text);
        }

        CaraReport::Span createSpan(const SourceLocation& location)
        {
            if (location.startIndex < 0 || location.endIndex < location.startIndex)
            {
                return CaraReport::Span{};
            }

            return CaraReport::Span::fromRange(location.startIndex, location.endIndex);
        }

        CaraReport::Label createLabel(const Diagnostic::Label& label)
        {
            return CaraReport::Label(
                createSpan(label.location),
                label.text,
                label.isPrimary);
        }

        const SourceText& diagnosticSource(const Diagnostic& diagnostic, const SourceText& fallbackSource)
        {
            if (diagnostic.source)
            {
                return *diagnostic.source;
            }

            return fallbackSource;
        }

        CaraReport::Report createReport(const Diagnostic& diagnostic, const SourceText& fallbackSource)
        {
            const auto& sourceText = diagnosticSource(diagnostic, fallbackSource);
            auto report = CaraReport::Report(diagnostic.message)
                .withTitle(diagnostic.code)
                .withLevel(toCaraReportLevel(diagnostic.level))
                .withSource(std::make_unique<CaraReport::Source>(createSource(sourceText)));

            const auto span = createSpan(diagnostic.location);
            if (!span.isEmpty())
            {
                static_cast<void>(report.withLabel(CaraReport::Label(span, diagnostic.message, true)));
            }

            for (const auto& label : diagnostic.labels)
            {
                static_cast<void>(report.withLabel(createLabel(label)));
            }

            if (diagnostic.fix.has_value())
            {
                static_cast<void>(report.withFix(diagnostic.fix.value()));
            }

            return report;
        }
    }

    void writeDiagnostic(
        std::ostream& outStream,
        const Diagnostic& diagnostic,
        const SourceText& fallbackSource,
        bool enableColors,
        bool enableUnicode)
    {
        auto report = createReport(diagnostic, fallbackSource);
        auto writer = CaraReport::Writer::create()
            .withColors(enableColors)
            .withUnicode(enableUnicode);

        outStream << writer.writeReport(report);
    }

    void writeDiagnostics(
        std::ostream& outStream,
        DiagnosticsBag& diagnostics,
        const SourceText& fallbackSource,
        bool enableColors,
        bool enableUnicode)
    {
        auto writer = CaraReport::Writer::create()
            .withColors(enableColors)
            .withUnicode(enableUnicode);

        const auto& entries = diagnostics.Diagnostics();
        for (std::size_t i = 0; i < entries.size(); ++i)
        {
            auto report = createReport(entries[i], fallbackSource);
            outStream << writer.writeReport(report);
        }
    }

    std::string renderDiagnostic(
        const Diagnostic& diagnostic,
        const SourceText& source,
        bool enableColors,
        bool enableUnicode)
    {
        std::ostringstream outStream;
        writeDiagnostic(outStream, diagnostic, source, enableColors, enableUnicode);
        return outStream.str();
    }

    std::string renderDiagnostics(
        DiagnosticsBag& diagnostics,
        const SourceText& source,
        bool enableColors,
        bool enableUnicode)
    {
        std::ostringstream outStream;
        writeDiagnostics(outStream, diagnostics, source, enableColors, enableUnicode);
        return outStream.str();
    }
}