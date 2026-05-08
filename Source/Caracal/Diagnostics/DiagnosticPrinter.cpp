#include <Caracal/Diagnostics/DiagnosticPrinter.h>
#include <CaraReport.h>

#include <sstream>

namespace Caracal
{
    static CaraReport::Level ToCaraReportLevel(DiagnosticLevel level)
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

    static const SourceText& ResolveSource(
        const SourceTextSharedPtr& source,
        const SourceTextSharedPtr& diagnosticSource)
    {
        if (source)
        {
            return *source;
        }

        return *diagnosticSource;
    }

    static std::unique_ptr<CaraReport::Source> CreateSource(const SourceText& source)
    {
        if (source.filePath.empty())
        {
            return std::make_unique<CaraReport::Source>("<input>", source.text);
        }

        return std::make_unique<CaraReport::Source>(source.filePath.generic_string(), source.text);
    }

    // todo handle this in the carareport lib
    static SourceLocation FixUpLocation(const SourceLocation& location, const SourceText& source)
    {
        if (location.startIndex < 0 || location.endIndex < location.startIndex)
        {
            return {};
        }

        if (location.startIndex == location.endIndex && location.startIndex > 0)
        {
            const auto sourceSize = static_cast<i32>(source.text.size());
            if (location.startIndex <= sourceSize)
            {
                return {
                    .startIndex = location.startIndex - 1,
                    .endIndex = location.startIndex
                };
            }
        }

        return location;
    }

    static CaraReport::Span CreateSpan(const SourceLocation& location)
    {
        if (location.startIndex < 0 || location.endIndex < location.startIndex)
        {
            return CaraReport::Span{};
        }

        return CaraReport::Span::fromRange(location.startIndex, location.endIndex);
    }

    static CaraReport::Label CreateLabel(
        const Diagnostic::Label& label,
        const SourceTextSharedPtr& diagnosticSource)
    {
        const auto& sourceText = ResolveSource(label.source, diagnosticSource);
        const auto renderLocation = FixUpLocation(label.location, sourceText);
        return CaraReport::Label(
            CreateSpan(renderLocation),
            label.text,
            label.isPrimary);
    }

    static CaraReport::Report CreateReport(
        const Diagnostic& diagnostic,
        const SourceTextSharedPtr& diagnosticSource)
    {
        return CaraReport::Report(diagnostic.message())
            .withTitle(std::string(diagnostic.code()))
            .withLevel(ToCaraReportLevel(diagnostic.level()))
            .withSource(CreateSource(*diagnosticSource));
    }

    static CaraReport::Report BuildReport(const Diagnostic& diagnostic)
    {
        const auto& diagnosticSource = diagnostic.source();
        auto report = CreateReport(diagnostic, diagnosticSource);

        for (const auto& label : diagnostic.labels())
        {
            static_cast<void>(report.withLabel(CreateLabel(label, diagnosticSource)));
        }

        if (diagnostic.fix().has_value())
        {
            static_cast<void>(report.withFix(diagnostic.fix().value()));
        }

        return report;
    }

    void writeDiagnostic(
        std::ostream& outStream,
        const Diagnostic& diagnostic,
        const DiagnosticOptions& options)
    {
        auto report = BuildReport(diagnostic);
        auto writer = CaraReport::Writer::create()
            .withContextLines(options.contextLines)
            .withColors(options.enableColors)
            .withUnicode(options.enableUnicode);

        outStream << writer.writeReport(report);
    }

    void writeDiagnostics(
        std::ostream& outStream,
        DiagnosticsBag& diagnostics,
        const DiagnosticOptions& options)
    {
        auto writer = CaraReport::Writer::create()
            .withContextLines(options.contextLines)
            .withColors(options.enableColors)
            .withUnicode(options.enableUnicode);

        const auto& entries = diagnostics.Diagnostics();
        for (std::size_t i = 0; i < entries.size(); ++i)
        {
            auto report = BuildReport(entries[i]);
            outStream << writer.writeReport(report);
        }
    }

    std::string formatDiagnostic(
        const Diagnostic& diagnostic,
        const DiagnosticOptions& options)
    {
        std::ostringstream outStream;
        writeDiagnostic(outStream, diagnostic, options);
        return outStream.str();
    }

    std::string formatDiagnostics(
        DiagnosticsBag& diagnostics,
        const DiagnosticOptions& options)
    {
        std::ostringstream outStream;
        writeDiagnostics(outStream, diagnostics, options);
        return outStream.str();
    }
}
