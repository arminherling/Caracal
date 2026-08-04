#include <CaraTest.h>
#include <Caracal/Diagnostics/DiagnosticsBag.h>
#include <Caracal/Text/SourceEncoding.h>
#include <Caracal/Debug/ParseTreePrinter.h>
#include <Caracal/Semantic/TypeChecker.h>
#include <Caracal/Optimization/ConstantFolder.h>
#include <Caracal/Semantic/TypeCheckerOptions.h>
#include <Caracal/Semantic/SemanticContext.h>
#include <Caracal/Syntax/Lexer.h>
#include <Caracal/Syntax/Parser.h>
#include <Caracal/Text/File.h>
#include <iostream>

using namespace CaraTest;

static void FileTests(
    const std::string& fileName,
    const std::filesystem::path& inputFilePath,
    const std::filesystem::path& outputFilePath)
{
    if (!std::filesystem::exists(inputFilePath))
        CaraTest::fail();// ("In file missing");
    if (!std::filesystem::exists(outputFilePath))
        CaraTest::skip();// ("Out file missing");

    const auto input = Caracal::File::readText(inputFilePath);
    if (!input.has_value())
        CaraTest::fail();// ("Could not read input file");

    const auto source = std::make_shared<Caracal::SourceText>(input.value());
    Caracal::DiagnosticsBag diagnostics;

    if (!Caracal::validateSourceEncoding(source, diagnostics))
        CaraTest::fail();

    const auto tokens = Caracal::lex(source, diagnostics);
    auto parseTree = Caracal::parse(tokens, diagnostics);
    std::vector<Caracal::ParseTreeUPtr> parseTrees;
    parseTrees.push_back(std::move(parseTree));

    Caracal::TypeCheckerOptions options{
        .defaultIntegerType = "i32",
        .defaultFloatingType = "f32",
        .defaultEnumBaseType = "u8"
    };
    const auto preludePath = std::filesystem::path(__FILE__).parent_path() / "../../../Prelude";
    const auto preludeSources = Caracal::SemanticContext::CollectPreludeSources(preludePath);
    Caracal::SemanticContext module = Caracal::SemanticContext::WithBuiltins(preludeSources, options);

    auto startTime = std::chrono::high_resolution_clock::now();
    auto wasSuccessful = Caracal::typeCheck(parseTrees, options, module, diagnostics);
    if (wasSuccessful)
    {
        wasSuccessful = Caracal::foldConstants(parseTrees, module, diagnostics);
    }
    auto endTime = std::chrono::high_resolution_clock::now();

    std::cout << "      Type check(): " << CaraTest::stringify(endTime - startTime) << std::endl;

    if (!wasSuccessful) {
        CaraTest::fail();// ("Type checking failed");
    }

    Caracal::ParseTreePrinter printer{ *parseTrees[0], &module};
    const auto output = printer.prettyPrint();

    CaraTest::isTrue(!diagnostics.hasErrors());
    const auto outputDirectory = std::filesystem::path(outputFilePath).parent_path();
    if (!std::filesystem::exists(outputDirectory))
    {
        std::filesystem::create_directories(outputDirectory);
    }

    CaraTest::equalsFile(std::filesystem::path(outputFilePath), output);
}

static std::vector<std::tuple<std::string, std::filesystem::path, std::filesystem::path>> FileTests_Data()
{
    const auto currentFilePath = std::filesystem::path(__FILE__);
    const auto currentDirectory = currentFilePath.parent_path();
    const auto testDataDir = currentDirectory / "../../TestData";
    const auto inputDir = testDataDir / "Input";
    const auto absolutePath = std::filesystem::absolute(inputDir);

    std::vector<std::tuple<std::string, std::filesystem::path, std::filesystem::path>> data{};

    if (!std::filesystem::exists(absolutePath))
    {
        return data;
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(absolutePath))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".cara")
        {
            const auto& inputFilePath = entry.path();
            const auto inputDirName = inputFilePath.parent_path().filename();

            const auto outputParseDirectoryPath = testDataDir / "OutputType";

            const auto fileName = inputFilePath.stem().string();
            const auto outputFileName = fileName + ".txt";

            const auto putputParsePath = std::filesystem::absolute(outputParseDirectoryPath / inputDirName / outputFileName);

            const auto testName = inputDirName.string() + "/" + fileName;
            data.push_back(std::make_tuple(
                testName,
                inputFilePath,
                putputParsePath));
        }
    }
    return data;
}

static auto tests =
{
    CaraTest::addTest("FileTests", FileTests, FileTests_Data),
};
