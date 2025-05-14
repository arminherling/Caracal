#include <Compiler/File.h>
#include <Debug/ParseTreePrinter.h>
#include <CodeGen/CppCodeGenerator.h>
#include <Syntax/Lexer.h>
#include <Syntax/Token.h>
#include <Syntax/TokenKind.h>
#include <Syntax/TokenBuffer.h>
#include <Syntax/Parser.h>

#include <QString>
#include <QFileInfo>
#include <QTemporaryFile>
#include <QProcess>
#include <QDir>

#include <iostream>
#include <memory>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Please provide a file path as a parameter.\n";
        return -1;
    }

    auto filePath = QString::fromLocal8Bit(argv[1]);
    auto fileInfo = QFileInfo(filePath);
    if (!fileInfo.isFile())
    {
        std::cout << "The parameter is not a valid file.\n";
        return -1;
    }
    
    auto absolutePath = fileInfo.absoluteFilePath();
    auto fileContent = Caracal::File::readAllText(absolutePath);
    auto source = std::make_shared<Caracal::SourceText>(fileContent);
    Caracal::DiagnosticsBag diagnostics;

    auto tokens = Caracal::lex(source, diagnostics);
    auto parseTree = Caracal::parse(tokens, diagnostics);
    auto cppCode = Caracal::generateCpp(parseTree);

    if (!diagnostics.Diagnostics().empty())
    {
        std::cout << "Errors found during parsing!";
        return -1;
    }

    QTemporaryFile tempFile;
    if (!tempFile.open())
    {
        std::cout << "Failed to create temporary file.\n";
        return -1;
    }

    auto sourceFilePath = tempFile.fileName();
    tempFile.write(cppCode.toUtf8());
    tempFile.close();

    auto baseName = fileInfo.baseName();
    auto executablePath = baseName + ".exe";
    auto arguments = QStringList()
        << "-x"                    // Specify language flag
        << "c++"                   // Specify C++ language
        << sourceFilePath
        << "-std=c++17"            // Specify C++17 standard
        << "-O2"                   // Standard optimization
        << "-Wall"                 // Enable most warnings
        << "-Wextra"               // Enable additional warnings
        << "-o" << executablePath; // Specify output executable

    QProcess clangProcess;
    QObject::connect(&clangProcess, &QProcess::readyReadStandardOutput, 
        [&clangProcess]()
        {
            std::cout << clangProcess.readAllStandardOutput().toStdString();
        });
    QObject::connect(&clangProcess, &QProcess::readyReadStandardError, 
        [&clangProcess]()
        {
            std::cout << clangProcess.readAllStandardError().toStdString();
        });

    std::cout << "Compiling...\n";
    clangProcess.start("clang++", arguments);
    clangProcess.waitForFinished();

    if (clangProcess.exitStatus() != QProcess::NormalExit || clangProcess.exitCode() != 0)
    {
        return -1;
    }
    std::cout << "Compiling successful.\n";

    QProcess programProcess;
    QObject::connect(&programProcess, &QProcess::readyReadStandardOutput, 
        [&programProcess]() 
        {
            std::cout << programProcess.readAllStandardOutput().toStdString();
        });

    std::cout << "\nExecuting...\n";
    programProcess.start(executablePath);
    programProcess.waitForFinished();

    return 0;
}
