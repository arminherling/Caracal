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
    const std::string& inputFilePath, 
    const std::string& outputFilePath, 
    const std::string& errorFilePath)
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

static QList<std::tuple<std::string, std::string, std::string, std::string>> FileTests_Data()
{
    auto testDataDir = QDir(QString("../../Tests/Data"));
    auto absolutePath = testDataDir.absolutePath();

    QList<std::tuple<std::string, std::string, std::string, std::string>> data{};

    QDirIterator it(absolutePath, QStringList() << QString("*.aal"), QDir::Filter::Files, QDirIterator::IteratorFlag::Subdirectories);
    while (it.hasNext())
    {
        auto file = QFileInfo(it.next());
        auto directory = QDir(file.absolutePath());
        auto fullFilePathWithoutExtension = directory.filePath(file.baseName());

        auto inPath = QDir::cleanPath(fullFilePathWithoutExtension + QString(".aal"));
        auto outPath = QDir::cleanPath(fullFilePathWithoutExtension + QString(".out_type"));
        auto errorPath = QDir::cleanPath(fullFilePathWithoutExtension + QString(".error_type"));

        auto testName = directory.dirName() + '/' + file.completeBaseName();
        data.append(std::make_tuple(testName.toStdString(), inPath.toStdString(), outPath.toStdString(), errorPath.toStdString()));
    }
    return data;
}

static auto tests =
{
    CaraTest::addTest("FileTests", FileTests, FileTests_Data),
};
