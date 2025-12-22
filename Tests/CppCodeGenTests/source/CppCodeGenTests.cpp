#include <CaraTest.h>
#include <CodeGen/CppCodeGenerator.h>
#include <Compiler/DiagnosticsBag.h>
#include <Compiler/File.h>
#include <iostream>
#include <QDirIterator>
#include <Syntax/Lexer.h>
#include <Syntax/Parser.h>

static void FileTests(
    const std::string& /*fileName*/, 
    const std::filesystem::path& inputFilePath, 
    const std::filesystem::path& outputFilePath, 
    const std::filesystem::path& /*errorFilePath*/)
{
    if (!QFile::exists(inputFilePath))
        CaraTest::fail();// ("In file missing");
    if (!QFile::exists(outputFilePath))
        CaraTest::skip();// ("Out file missing");

    const auto input = Caracal::File::readText(inputFilePath);
    if (!input.has_value())
        CaraTest::fail();// ("Could not read input file");

    const auto source = std::make_shared<Caracal::SourceText>(input.value());
    Caracal::DiagnosticsBag diagnostics;

    const auto tokens = Caracal::lex(source, diagnostics);
    const auto parseTree = Caracal::parse(tokens, diagnostics);

    const auto startTime = std::chrono::high_resolution_clock::now();
    const auto output = Caracal::generateCpp(parseTree);
    const auto endTime = std::chrono::high_resolution_clock::now();

    std::cout << "      generateCpp(): " << CaraTest::stringify(endTime - startTime) << std::endl;

    CaraTest::isTrue(diagnostics.Diagnostics().empty());
    CaraTest::equalsFile(std::filesystem::path(outputFilePath), output);
}

static std::vector<std::tuple<std::string, std::filesystem::path, std::filesystem::path, std::filesystem::path>> FileTests_Data()
{
    const auto currentFilePath = std::filesystem::path(__FILE__);
    const auto currentDirectory = currentFilePath.parent_path();
    const auto testDataDir = currentDirectory / "../../Data";
    const auto absolutePath = std::filesystem::absolute(testDataDir);

    std::vector<std::tuple<std::string, std::filesystem::path, std::filesystem::path, std::filesystem::path>> data;

    for (const auto& entry : std::filesystem::recursive_directory_iterator(absolutePath))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".cara")
        {
            const auto& filePath = entry.path();
            const auto parentPath = filePath.parent_path();
            const auto baseName = filePath.stem(); // file name without extension
            
            const auto& inPath = filePath;
            const auto outPath = parentPath / (baseName.string() + ".out_cpp");
            const auto errorPath = parentPath / (baseName.string() + ".error_cpp");
            
            const auto testName = parentPath.filename().string() + '/' + baseName.string();
            data.emplace_back(testName, inPath, outPath, errorPath);
        }
    }

    return data;
}

static auto tests =
{
    CaraTest::addTest("FileTests", FileTests, FileTests_Data),
};
