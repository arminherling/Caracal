#include <CaraTest.h>
#include <Caracal/Diagnostics/DiagnosticsBag.h>
#include <Caracal/Semantic/Module.h>
#include <Caracal/Semantic/TypeChecker.h>
#include <Caracal/Semantic/TypeCheckerOptions.h>
#include <Caracal/Syntax/Lexer.h>
#include <Caracal/Syntax/Parser.h>
#include <Caracal/Text/File.h>
#include <iostream>

static void FileTests(
    const std::string& /*fileName*/,
    const std::filesystem::path& inputFilePath,
    const std::filesystem::path& outputFilePath,
    const std::filesystem::path& /*errorFilePath*/)
{
    if (!std::filesystem::exists(inputFilePath))
        CaraTest::fail();// ("In file missing");

    CaraTest::skip();

    /*
    if (!std::filesystem::exists(outputFilePath))
        CaraTest::skip();// ("Out file missing");

    const auto input = Caracal::File::readText(inputFilePath);
    if (!input.has_value())
        CaraTest::fail();// ("Could not read input file");

    const auto source = std::make_shared<Caracal::SourceText>(input.value());
    Caracal::DiagnosticsBag diagnostics;

    const auto tokens = Caracal::lex(source, diagnostics);
    auto parseTree = Caracal::parse(tokens, diagnostics);
    std::vector<Caracal::ParseTreeUPtr> parseTrees;
    parseTrees.push_back(std::move(parseTree));

    Caracal::Module module = Caracal::Module::WithBuiltins();
    Caracal::TypeCheckerOptions options{
        .defaultIntegerType = Caracal::Type::I32(),
        .defaultFloatingType = Caracal::Type::F32(),
        .defaultEnumBaseType = Caracal::Type::U8()
    };

    auto wasSuccessful = Caracal::typeCheck(parseTrees, options, module, diagnostics);
    if (!wasSuccessful)
    {
        std::cout << "Type checking failed!";
        CaraTest::fail();
    }

    const auto startTime = std::chrono::high_resolution_clock::now();
    const auto [irGenerated, output] = Caracal::generateIRText(*parseTrees[0], module);
    const auto endTime = std::chrono::high_resolution_clock::now();

    std::cout << "      generateIRText(): " << CaraTest::stringify(endTime - startTime) << std::endl;

    CaraTest::isTrue(irGenerated);
    CaraTest::isTrue(diagnostics.Diagnostics().empty());
    CaraTest::equalsFile(std::filesystem::path(outputFilePath), output);
    */
}

static std::vector<std::tuple<std::string, std::filesystem::path, std::filesystem::path, std::filesystem::path>> FileTests_Data()
{
    const auto currentFilePath = std::filesystem::path(__FILE__);
    const auto currentDirectory = currentFilePath.parent_path();
    const auto testDataDir = currentDirectory / "../../TestData";
    const auto inputDir = testDataDir / "Input";
    const auto absolutePath = std::filesystem::absolute(inputDir);

    std::vector<std::tuple<std::string, std::filesystem::path, std::filesystem::path, std::filesystem::path>> data{};

    for (const auto& entry : std::filesystem::recursive_directory_iterator(absolutePath))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".cara")
        {
            const auto& inputFilePath = entry.path();
            const auto inputDirName = inputFilePath.parent_path().filename();

            const auto outputParseDirectoryPath = testDataDir / "OutputIR";
            if (!std::filesystem::exists(outputParseDirectoryPath))
            {
                std::filesystem::create_directories(outputParseDirectoryPath);
            }

            const auto fileName = inputFilePath.stem().string();
            const auto outputFileName = fileName + ".txt";
            const auto outputFileNameError = fileName + "_error.txt";

            const auto outputParsePath = std::filesystem::absolute(outputParseDirectoryPath / inputDirName / outputFileName);
            const auto outputErrorPath = std::filesystem::absolute(outputParseDirectoryPath / inputDirName / outputFileNameError);

            const auto testName = inputDirName.string() + "/" + fileName;
            data.push_back(std::make_tuple(
                testName,
                inputFilePath,
                outputParsePath,
                outputErrorPath));
        }
    }
    return data;
}

static auto tests =
{
    CaraTest::addTest("FileTests", FileTests, FileTests_Data),
};
