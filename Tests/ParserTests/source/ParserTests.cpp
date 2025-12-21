#include <CaraTest.h>
#include <Compiler/DiagnosticsBag.h>
#include <Compiler/File.h>
#include <Debug/ParseTreePrinter.h>
#include <iostream>
#include <QDirIterator>
#include <Syntax/Lexer.h>
#include <Syntax/Parser.h>

static void FileTests(
    const std::string& fileName, 
    const std::string& inputFilePath, 
    const std::string& outputFilePath, 
    const std::string& errorFilePath)
{
    if (!QFile::exists(QString::fromStdString(inputFilePath)))
        CaraTest::fail();// ("In file missing");
    if (!QFile::exists(QString::fromStdString(outputFilePath)))
        CaraTest::skip();// ("Out file missing");

    auto input = Caracal::File::ReadAllText(QString::fromStdString(inputFilePath));
    auto source = std::make_shared<Caracal::SourceText>(input.toStdString());
    Caracal::DiagnosticsBag diagnostics;

    auto tokens = Caracal::lex(source, diagnostics);

    auto startTime = std::chrono::high_resolution_clock::now();
    auto parseTree = Caracal::parse(tokens, diagnostics);
    auto endTime = std::chrono::high_resolution_clock::now();

    std::cout << "      parse(): " << CaraTest::stringify(endTime - startTime) << std::endl;

    Caracal::ParseTreePrinter printer{ parseTree };
    auto output = printer.prettyPrint().toStdString();

    CaraTest::isTrue(diagnostics.Diagnostics().empty());
    CaraTest::equalsFile(std::filesystem::path(outputFilePath), output);
}

static QList<std::tuple<std::string, std::string, std::string, std::string>> FileTests_Data()
{
    auto testDataDir = QDir(QString("../../Tests/Data"));
    auto absolutePath = testDataDir.absolutePath();

    QList<std::tuple<std::string, std::string, std::string, std::string>> data{};

    QDirIterator it(absolutePath, QStringList() << QString("*.cara"), QDir::Filter::Files, QDirIterator::IteratorFlag::Subdirectories);
    while (it.hasNext())
    {
        auto file = QFileInfo(it.next());
        auto directory = QDir(file.absolutePath());
        auto fullFilePathWithoutExtension = directory.filePath(file.baseName());

        auto inPath = QDir::cleanPath(fullFilePathWithoutExtension + QString(".cara"));
        auto outPath = QDir::cleanPath(fullFilePathWithoutExtension + QString(".out_parse"));
        auto errorPath = QDir::cleanPath(fullFilePathWithoutExtension + QString(".error_parse"));

        auto testName = directory.dirName() + '/' + file.completeBaseName();
        data.append(std::make_tuple(testName.toStdString(), inPath.toStdString(), outPath.toStdString(), errorPath.toStdString()));
    }
    return data;
}

static auto tests =
{
    CaraTest::addTest("FileTests", FileTests, FileTests_Data),
};
