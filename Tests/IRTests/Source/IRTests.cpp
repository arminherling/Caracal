#include <CaraTest.h>

#include <Caracal/Compiler.h>
#include <Caracal/Diagnostics/DiagnosticsBag.h>
#include <Caracal/Semantic/SemanticContext.h>
#include <Caracal/Semantic/TypeChecker.h>
#include <Caracal/Optimization/ConstantFolder.h>
#include <Caracal/Semantic/TypeCheckerOptions.h>
#include <Caracal/Syntax/Lexer.h>
#include <Caracal/Syntax/Parser.h>
#include <Caracal/Text/File.h>

#include <iostream>

static void FileTests(
    const std::string& /*fileName*/,
    const std::filesystem::path& inputFilePath,
    const std::filesystem::path& outputFilePath)
{
    if (!std::filesystem::exists(inputFilePath))
        CaraTest::fail();
    if (!std::filesystem::exists(outputFilePath))
        CaraTest::skip();

    const auto input = Caracal::File::readText(inputFilePath);
    if (!input.has_value())
        CaraTest::fail();

    const auto source = std::make_shared<Caracal::SourceText>(input.value());
    Caracal::DiagnosticsBag diagnostics;

    const auto tokens = Caracal::lex(source, diagnostics);
    auto parseTree = Caracal::parse(tokens, diagnostics);
    std::vector<Caracal::ParseTreeUPtr> parseTrees;
    parseTrees.push_back(std::move(parseTree));

    Caracal::TypeCheckerOptions options{
        .defaultIntegerType = Caracal::Type::I32(),
        .defaultFloatingType = Caracal::Type::F32(),
        .defaultEnumBaseType = Caracal::Type::U8()
    };
    const auto preludePath = std::filesystem::path(__FILE__).parent_path() / "../../../Prelude";
    const auto preludeSources = Caracal::SemanticContext::CollectPreludeSources(preludePath);
    Caracal::SemanticContext module = Caracal::SemanticContext::WithBuiltins(preludeSources, options);

    auto wasSuccessful = Caracal::typeCheck(parseTrees, options, module, diagnostics);
    if (wasSuccessful)
    {
        wasSuccessful = Caracal::foldConstants(parseTrees, module, diagnostics);
    }
    if (!wasSuccessful)
    {
        std::cout << "Type checking failed!";
        CaraTest::fail();
    }

    const auto startTime = std::chrono::high_resolution_clock::now();
    const auto [irGenerated, output] = Caracal::generateIRText(module);
    const auto endTime = std::chrono::high_resolution_clock::now();

    std::cout << "      generateIRText(): " << CaraTest::stringify(endTime - startTime) << std::endl;

    CaraTest::isTrue(irGenerated);
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

            const auto outputParseDirectoryPath = testDataDir / "OutputIR";

            const auto fileName = inputFilePath.stem().string();
            const auto outputFileName = fileName + ".txt";
            const auto outputParsePath = std::filesystem::absolute(outputParseDirectoryPath / inputDirName / outputFileName);

            const auto testName = inputDirName.string() + "/" + fileName;
            data.push_back(std::make_tuple(testName, inputFilePath, outputParsePath));
        }
    }

    return data;
}

static auto tests =
{
    CaraTest::addTest("FileTests", FileTests, FileTests_Data),
};
