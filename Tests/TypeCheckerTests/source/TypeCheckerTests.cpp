#include <CaraTest.h>
#include <Compiler/DiagnosticsBag.h>
#include <Compiler/File.h>
#include <Debug/TypedTreePrinter.h>
#include <iostream>
#include <QDirIterator>
#include <Semantic/TypeChecker.h>
#include <Semantic/TypeCheckerOptions.h>
#include <Semantic/TypeDatabase.h>
#include <Syntax/Lexer.h>
#include <Syntax/Parser.h>

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
            const auto outPath = parentPath / (baseName.string() + ".out_type");
            const auto errorPath = parentPath / (baseName.string() + ".error_type");

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
