#include <CaraTest.h>
#include <Caracal/Debug/DiagnosticsBag.h>
#include <Caracal/Debug/TypedTreePrinter.h>
#include <Caracal/Semantic/TypeChecker.h>
#include <Caracal/Semantic/TypeCheckerOptions.h>
#include <Caracal/Semantic/TypeDatabase.h>
#include <Caracal/Syntax/Lexer.h>
#include <Caracal/Syntax/Parser.h>
#include <Caracal/Text/File.h>
#include <iostream>

using namespace CaraTest;

static void FileTests(
    const std::string& fileName, 
    const std::filesystem::path& inputFilePath, 
    const std::filesystem::path& outputFilePath, 
    const std::filesystem::path& errorFilePath)
{
    CaraTest::skip();

    //if (!QFile::exists(inputFilePath))
    //    CaraTest::Fail();// ("In file missing");
    //if (!QFile::exists(outputFilePath))
    //    CaraTest::Skip();// ("Out file missing");

    //auto input = File::ReadAllText(inputFilePath);
    //auto source = std::make_shared<SourceText>(input);
    //DiagnosticsBag diagnostics;

    //auto tokens = Lex(source, diagnostics);
    //auto parseTree = Parse(tokens, diagnostics);

    //TypeDatabase typeDatabase;
    //TypeCheckerOptions options{
    //    .defaultIntegerType = Type::I32(),
    //    .defaultEnumBaseType = Type::U8()
    //};

    //auto startTime = std::chrono::high_resolution_clock::now();
    //auto typedTree = TypeCheck(parseTree, options, typeDatabase, diagnostics);
    //auto endTime = std::chrono::high_resolution_clock::now();

    //std::cout << "      Type check(): " << stringify(endTime - startTime).toStdString() << std::endl;

    //TypedTreePrinter printer{ typedTree, typeDatabase };
    //auto output = printer.PrettyPrint();
    //auto expectedOutput = File::ReadAllText(outputFilePath);

    //CaraTest::areEqual(expectedOutput, output);
    //if (!QFile::exists(errorFilePath))
    //{
    //    // TODO maybe split up errors and warnings
    //    for (const auto& diagnostic : diagnostics.Diagnostics())
    //    {
    //        if (diagnostic.level == DiagnosticLevel::Error)
    //        {
    //            CaraTest::Fail();
    //            break;
    //        }
    //    }
    //}
    //else
    //{
    //    CaraTest::Fail();// ("TODO compare errors with error file once we got some");
    //}
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

            const auto outputParseDirectoryPath = testDataDir / "OutputType";
            if (!std::filesystem::exists(outputParseDirectoryPath))
            {
                std::filesystem::create_directories(outputParseDirectoryPath);
            }

            const auto fileName = inputFilePath.stem().string();
            const auto extension = inputFilePath.extension().string();
            const auto outputFileName = fileName + ".txt";
            const auto outputFileNameError = fileName + "_error.txt";

            const auto putputParsePath = std::filesystem::absolute(outputParseDirectoryPath / inputDirName / outputFileName);
            const auto putputErrorPath = std::filesystem::absolute(outputParseDirectoryPath / inputDirName / outputFileNameError);

            const auto testName = inputDirName.string() + "/" + fileName;
            data.push_back(std::make_tuple(
                testName,
                inputFilePath,
                putputParsePath,
                putputErrorPath));
        }
    }
    return data;
}

static auto tests =
{
    CaraTest::addTest("FileTests", FileTests, FileTests_Data),
};
