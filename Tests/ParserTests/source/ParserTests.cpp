#include "ParserTests.h"
#include <CaraTest.h>
#include <Compiler/DiagnosticsBag.h>
#include <Compiler/File.h>
#include <Debug/ParseTreePrinter.h>
#include <iostream>
#include <QDirIterator>
#include <Syntax/Lexer.h>
#include <Syntax/Parser.h>

namespace
{
    void FileTests(const QString& fileName, const QString& inputFilePath, const QString& outputFilePath, const QString& errorFilePath)
    {
        if (!QFile::exists(inputFilePath))
            CaraTest::Fail();// ("In file missing");
        if (!QFile::exists(outputFilePath))
            CaraTest::Skip();// ("Out file missing");

        auto input = Caracal::File::ReadAllText(inputFilePath);
        auto source = std::make_shared<Caracal::SourceText>(input);
        Caracal::DiagnosticsBag diagnostics;

        auto tokens = Caracal::lex(source, diagnostics);

        auto startTime = std::chrono::high_resolution_clock::now();
        auto parseTree = Caracal::parse(tokens, diagnostics);
        auto endTime = std::chrono::high_resolution_clock::now();

        std::cout << "      parse(): " << CaraTest::Stringify(endTime - startTime).toStdString() << std::endl;

        Caracal::ParseTreePrinter printer{ parseTree };
        auto output = printer.prettyPrint();

        CaraTest::IsTrue(diagnostics.Diagnostics().empty());
        CaraTest::EqualsFile(QFileInfo(outputFilePath), output);
    }

    QList<std::tuple<QString, QString, QString, QString>> FileTests_Data()
    {
        auto testDataDir = QDir(QString("../../Tests/Data"));
        auto absolutePath = testDataDir.absolutePath();

        QList<std::tuple<QString, QString, QString, QString>> data{};

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
            data.append(std::make_tuple(testName, inPath, outPath, errorPath));
        }
        return data;
    }
}

CaraTest::TestSuite ParserTestsSuite()
{
    CaraTest::TestSuite suite{};

    suite.add(QString("FileTests"), FileTests, FileTests_Data);

    return suite;
}
