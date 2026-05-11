#include <CaraTest.h>

#include <Caracal/Diagnostics/DiagnosticPrinter.h>
#include <Caracal/Diagnostics/DiagnosticsBag.h>
#include <Caracal/Semantic/Module.h>
#include <Caracal/Semantic/Type.h>
#include <Caracal/Semantic/TypeChecker.h>
#include <Caracal/Semantic/TypeCheckerOptions.h>
#include <Caracal/Syntax/Lexer.h>
#include <Caracal/Syntax/Parser.h>
#include <Caracal/Text/File.h>
#include <Caracal/Text/SourceText.h>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <tuple>
#include <vector>

namespace
{
    using DiagnosticsTestData = std::tuple<std::string, std::filesystem::path, std::filesystem::path>;

    std::filesystem::path CurrentDirectory()
    {
        return std::filesystem::path(__FILE__).parent_path();
    }

    std::filesystem::path DiagnosticsDataDirectory()
    {
        return std::filesystem::absolute(CurrentDirectory() / "../../TestData/Diagnostics");
    }

    std::filesystem::path TestDataDirectory()
    {
        return std::filesystem::absolute(CurrentDirectory() / "../../TestData");
    }

    std::filesystem::path DiagnosticsInputDirectory()
    {
        return DiagnosticsDataDirectory() / "Input";
    }

    std::filesystem::path DiagnosticsOutputDirectory()
    {
        return DiagnosticsDataDirectory() / "Output";
    }

    std::filesystem::path RepositoryRootDirectory()
    {
        return std::filesystem::absolute(CurrentDirectory() / "../../..");
    }

    std::filesystem::path MakeRepositoryRelativePath(const std::filesystem::path& filePath)
    {
        const auto absoluteFilePath = std::filesystem::absolute(filePath);
        auto relativePath = std::filesystem::relative(absoluteFilePath, TestDataDirectory());
        if (relativePath.empty())
        {
            relativePath = std::filesystem::relative(absoluteFilePath, RepositoryRootDirectory());
        }

        if (relativePath.empty())
            return filePath;

        return relativePath;
    }

    void AddParseTree(
        const std::filesystem::path& filePath,
        Caracal::DiagnosticsBag& diagnostics,
        std::vector<Caracal::ParseTreeUPtr>& parseTrees)
    {
        const auto input = Caracal::File::readText(filePath);
        if (!input.has_value())
            throw std::runtime_error("Could not read input file: " + filePath.string());

        const auto source = std::make_shared<Caracal::SourceText>(input.value(), MakeRepositoryRelativePath(filePath));
        const auto diagnosticCount = diagnostics.Diagnostics().size();
        const auto tokens = Caracal::lex(source, diagnostics);
        if (diagnostics.Diagnostics().size() != diagnosticCount)
            return;

        auto parseTree = Caracal::parse(tokens, diagnostics);
        parseTrees.push_back(std::move(parseTree));
    }

    std::string RenderDiagnosticsForFile(const std::filesystem::path& inputFilePath)
    {
        Caracal::DiagnosticsBag diagnostics;
        std::vector<Caracal::ParseTreeUPtr> parseTrees{};

        AddParseTree(inputFilePath, diagnostics, parseTrees);

        const Caracal::DiagnosticOptions diagnosticOptions{
            .contextLines = 3,
            .enableColors = false,
            .enableUnicode = false
        };

        if (!diagnostics.Diagnostics().empty())
            return Caracal::formatDiagnostics(diagnostics, diagnosticOptions);

        Caracal::Module caracalModule = Caracal::Module::WithBuiltins();
        const Caracal::TypeCheckerOptions options{
            .defaultIntegerType = Caracal::Type::I32(),
            .defaultFloatingType = Caracal::Type::F32(),
            .defaultEnumBaseType = Caracal::Type::U8()
        };

        const auto wasSuccessful = Caracal::typeCheck(parseTrees, options, caracalModule, diagnostics);
        if (!wasSuccessful || !diagnostics.Diagnostics().empty())
            return Caracal::formatDiagnostics(diagnostics, diagnosticOptions);

        return {};
    }

    void WriteTextFile(const std::filesystem::path& filePath, const std::string& content)
    {
        std::filesystem::create_directories(filePath.parent_path());

        std::ofstream outFile(filePath, std::ios::out | std::ios::binary | std::ios::trunc);
        if (!outFile.is_open())
            throw std::runtime_error("Could not write output file: " + filePath.string());

        outFile << content;
    }

    static void FileTests(
        const std::string& fileName,
        const std::filesystem::path& inputFilePath,
        const std::filesystem::path& outputFilePath)
    {
        static_cast<void>(fileName);

        if (!std::filesystem::exists(inputFilePath))
            CaraTest::fail();
        if (!std::filesystem::exists(outputFilePath))
            CaraTest::skip();

        const auto output = RenderDiagnosticsForFile(inputFilePath);
        if (output.empty())
            CaraTest::fail();

        CaraTest::equalsFile(outputFilePath, output);
    }

    static std::vector<DiagnosticsTestData> FileTests_Data()
    {
        const auto inputDirectory = DiagnosticsInputDirectory();
        const auto outputDirectory = DiagnosticsOutputDirectory();

        std::vector<DiagnosticsTestData> data{};
        if (!std::filesystem::exists(inputDirectory))
            return data;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(inputDirectory))
        {
            if (!entry.is_regular_file() || entry.path().extension() != ".cara")
                continue;

            auto relativeInputPath = std::filesystem::relative(entry.path(), inputDirectory);
            const auto fileName = entry.path().stem().string();
            auto outputFilePath = outputDirectory / relativeInputPath;
            outputFilePath.replace_extension(".txt");

            relativeInputPath.replace_extension();
            const auto testName = relativeInputPath.generic_string();

            data.push_back(std::make_tuple(
                testName,
                entry.path(),
                std::filesystem::absolute(outputFilePath)));
        }

        return data;
    }
}

static auto tests =
{
    CaraTest::addTest("FileTests", FileTests, FileTests_Data),
};
