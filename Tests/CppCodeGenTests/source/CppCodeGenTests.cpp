#include "CppCodeGenTests.h"
#include <AalTest.h>
#include <CodeGen/CppCodeGenerator.h>
#include <Compiler/DiagnosticsBag.h>
#include <Compiler/File.h>
#include <iostream>
#include <QDirIterator>
#include <Syntax/Lexer.h>
#include <Syntax/Parser.h>

namespace
{
    void FileTests(const QString& fileName, const QString& inputFilePath, const QString& outputFilePath, const QString& errorFilePath)
    {
        if (!QFile::exists(inputFilePath))
            AalTest::Fail();// ("In file missing");
        if (!QFile::exists(outputFilePath))
            AalTest::Skip();// ("Out file missing");

        auto input = Caracal::File::ReadAllText(inputFilePath);
        auto source = std::make_shared<Caracal::SourceText>(input);
        Caracal::DiagnosticsBag diagnostics;

        auto tokens = Caracal::lex(source, diagnostics);
        auto parseTree = Caracal::parse(tokens, diagnostics);

        auto startTime = std::chrono::high_resolution_clock::now();
        auto output = Caracal::generateCpp(parseTree);
        auto endTime = std::chrono::high_resolution_clock::now();

        std::cout << "      generateCpp(): " << AalTest::Stringify(endTime - startTime).toStdString() << std::endl;

        AalTest::IsTrue(diagnostics.Diagnostics().empty());
        AalTest::EqualsFile(output, QFileInfo(outputFilePath));
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
            auto outPath = QDir::cleanPath(fullFilePathWithoutExtension + QString(".out_cpp"));
            auto errorPath = QDir::cleanPath(fullFilePathWithoutExtension + QString(".error_cpp"));

            auto testName = directory.dirName() + '/' + file.completeBaseName();
            data.append(std::make_tuple(testName, inPath, outPath, errorPath));
        }
        return data;
    }
}

AalTest::TestSuite CppCodeGenTestsSuite()
{
    AalTest::TestSuite suite{};

    suite.add(QString("FileTests"), FileTests, FileTests_Data);

    return suite;
}
