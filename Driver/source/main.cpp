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

    const auto parameterFilePath = std::filesystem::path(argv[1]);
    const auto absolutePath = std::filesystem::absolute(parameterFilePath);
    if (!std::filesystem::exists(absolutePath))
    {
        std::cout << "The parameter is not a valid file.\n";
        return -1;
    }

    auto fileContent = Caracal::File::readText(absolutePath);
    if(!fileContent.has_value())
    {
        std::cout << "Failed to read the file content.\n";
        return -1;
    }

    auto source = std::make_shared<Caracal::SourceText>(fileContent.value());
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

    auto generatedCode = cppCode.toUtf8();
    tempFile.write(generatedCode);
    tempFile.close();

    auto sourceFilePath = tempFile.fileName();
    auto baseName = parameterFilePath.stem().string();
    auto executablePath = QString::fromStdString(baseName + ".exe");
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

    const auto executionStartTime = std::chrono::high_resolution_clock::now();

    programProcess.start(executablePath);
    programProcess.waitForFinished();

    const auto executionEndTime = std::chrono::high_resolution_clock::now();
    auto executionDuration = executionEndTime - executionStartTime;

    const auto seconds = duration_cast<std::chrono::seconds>(executionDuration);
    executionDuration -= seconds;
    const auto milliseconds = duration_cast<std::chrono::milliseconds>(executionDuration);

    std::cout << "\nExecution Time: " << seconds << " " << milliseconds << '\n';

    return 0;
}
