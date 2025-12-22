#include <Compiler/File.h>
#include <Debug/ParseTreePrinter.h>
#include <CodeGen/CppCodeGenerator.h>
#include <Syntax/Lexer.h>
#include <Syntax/Token.h>
#include <Syntax/TokenKind.h>
#include <Syntax/TokenBuffer.h>
#include <Syntax/Parser.h>

#include <memory>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <chrono>
#include <sstream>

static std::filesystem::path createTemporaryFile(const std::filesystem::path& directory)
{
    // create dir if not exists
    if (!std::filesystem::exists(directory))
    {
        std::filesystem::create_directories(directory);
    }

    // use timestamp to create unique filename
    auto now = std::chrono::system_clock::now();
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
    std::string filename = "tempfile_" + std::to_string(seconds.count()) + ".cpp";
    std::filesystem::path tempFilePath = directory / filename;

    std::ofstream tempFile(tempFilePath);
    if (!tempFile)
    {
        throw std::runtime_error("Failed to create temporary file.");
    }

    tempFile.close();
    return tempFilePath;
}

static int executeCommand(const std::string& command)
{
    return std::system(command.c_str());
}

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

    auto tempFilePath = createTemporaryFile(std::filesystem::temp_directory_path());
    std::ofstream tempFile(tempFilePath);
    if (!tempFile)
    {
        std::cout << "Failed to create temporary file.";
        return -1;
    }
    tempFile << cppCode;
    tempFile.close();

    // change the extension to exe
    auto executablePath = parameterFilePath;
    executablePath.replace_extension(".exe");

    // Prepare arguments
    std::ostringstream command;
    command << "clang++ "
        << "-x c++ "                // Specify language flag
        << tempFilePath.string() << " "
        << "-std=c++17 "            // Specify C++17 standard
        << "-O2 "                   // Standard optimization
        << "-Wall "                 // Enable most warnings
        << "-Wextra "               // Enable additional warnings
        << "-o " << executablePath; // Specify output executable

    std::cout << "Compiling...\n";
    auto clangResult = executeCommand(command.str());
    if (clangResult != 0)
    {
        std::cout << "Compilation failed with exit code: " << clangResult << '\n';
        return -1;
    }
    std::cout << "Compiling successful.\n";

    std::cout << "\nExecuting...\n";
    auto executionStartTime = std::chrono::high_resolution_clock::now();

    // execute the compiled program
    auto caracalResult = executeCommand(executablePath.string());
    if (caracalResult != 0)
    {
        std::cout << "Execution failed with exit code: " << caracalResult << '\n';
        return -1;
    }

    auto executionEndTime = std::chrono::high_resolution_clock::now();
    auto executionDuration = executionEndTime - executionStartTime;

    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(executionDuration);
    executionDuration -= seconds;
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(executionDuration);

    std::cout << "\nExecution Time: " << seconds << " " << milliseconds << '\n';

    // remove temp file
    std::filesystem::remove(tempFilePath);

    return 0;
}
