#include <Caracal/Text/File.h>
#include <Caracal/Debug/ParseTreePrinter.h>
#include <Caracal/CodeGen/CppCodeGenerator.h>
#include <Caracal/CodeGen/LLVMCodeGenerator.h>
#include <Caracal/Syntax/Lexer.h>
#include <Caracal/Syntax/Token.h>
#include <Caracal/Syntax/TokenKind.h>
#include <Caracal/Syntax/TokenBuffer.h>
#include <Caracal/Syntax/Parser.h>
#include <Caracal/Semantic/TypeChecker.h>
#include <Caracal/Semantic/TypeCheckerOptions.h>
#include <Caracal/Semantic/TypeDatabase.h>

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Program.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/MC/TargetRegistry.h>

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
        return 1;
    }

    const auto parameterFilePath = std::filesystem::path(argv[1]);
    const auto absolutePath = std::filesystem::absolute(parameterFilePath);
    const auto inputFileName = absolutePath.stem().string();
    if (!std::filesystem::exists(absolutePath))
    {
        std::cout << "The parameter is not a valid file.\n";
        return 1;
    }

    auto fileContent = Caracal::File::readText(absolutePath);
    if (!fileContent.has_value())
    {
        std::cout << "Failed to read the file content.\n";
        return 1;
    }

    auto source = std::make_shared<Caracal::SourceText>(fileContent.value());
    Caracal::DiagnosticsBag diagnostics;

    auto tokens = Caracal::lex(source, diagnostics);
    auto parseTree = Caracal::parse(tokens, diagnostics);

    if (!diagnostics.Diagnostics().empty())
    {
        std::cout << "Errors found during parsing!";
        return 1;
    }

    Caracal::TypeDatabase typeDatabase;
    Caracal::TypeCheckerOptions options{
        .defaultIntegerType = Caracal::Type::I32(),
        .defaultFloatingType = Caracal::Type::F32(),
        .defaultEnumBaseType = Caracal::Type::U8()
    };

    auto wasSuccessful = Caracal::typeCheck(parseTree, options, typeDatabase, diagnostics);
    if (!wasSuccessful)
    {
        std::cout << "Type checking failed!";
        return 1;
    }

    //auto cppCode = Caracal::generateCpp(parseTree);

    llvm::LLVMContext context;
    llvm::Module module(inputFileName, context);

    wasSuccessful = Caracal::generateLLVMModule(parseTree, module);
    if (!wasSuccessful)
    {
        std::cout << "Module not generated!";
        return 1;
    }

    std::string irOutput;
    llvm::raw_string_ostream irStream(irOutput);
    module.print(irStream, nullptr);
    irStream.flush();
    llvm::outs() << "Module IR:\n" << irOutput << "\n";

    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();

    std::string targetError;
    auto defaultTargetTriple = llvm::sys::getDefaultTargetTriple();
    llvm::outs() << "TargetTriple: "<<defaultTargetTriple << '\n';
    auto target = llvm::TargetRegistry::lookupTarget(defaultTargetTriple, targetError);
    auto triple = llvm::Triple(defaultTargetTriple);
    if (!target)
    {
        llvm::errs() << "Error: " << targetError << "\n";
        return 1;
    }

    auto CPU = "generic";
    auto features = "";
    llvm::TargetOptions targetOptions;
    auto relocModel = std::optional<llvm::Reloc::Model>();
    auto targetMachine = target->createTargetMachine(defaultTargetTriple, CPU, features, targetOptions, relocModel);
    module.setDataLayout(targetMachine->createDataLayout());

    const auto objectFileName = inputFileName + ".o";
    std::error_code errorCode;
    llvm::raw_fd_ostream objectFile(objectFileName, errorCode, llvm::sys::fs::OF_None);
    if (errorCode)
    {
        llvm::errs() << "Couldnt create object file: " << errorCode.message() << "\n";
        return 1;
    }

    llvm::legacy::PassManager passManager;
    if (targetMachine->addPassesToEmitFile(passManager, objectFile, nullptr, llvm::CodeGenFileType::ObjectFile))
    {
        llvm::errs() << "TargetMachine cant emit a file of this type\n";
        return 1;
    }
    passManager.run(module);
    objectFile.flush();

    const auto lldPath = llvm::sys::findProgramByName("lld-link").get();
    if (lldPath.empty())
    {
        llvm::errs() << "Error: lld-link not found in PATH.\n";
        return 1;
    }


    // TODO support linux
    std::string linkError;
    const auto exeFileName = inputFileName + ".exe";
    const auto linkingResult = llvm::sys::ExecuteAndWait(
        lldPath,
        { "lld-link", "-flavor", "link", "/out:" + exeFileName, objectFileName, "/subsystem:console", "/entry:main", "/defaultlib:msvcrt.lib", "/defaultlib:legacy_stdio_definitions.lib","/defaultlib:ucrt.lib" },
        std::nullopt,
        {},
        0,
        0,
        &linkError
    );

    if (linkingResult != 0)
    {
        llvm::errs() << "Linking failed with error code: " << linkingResult << "\n";
        llvm::errs() << "Error message: " << linkError << "\n";
        return 0;
    }
    else
    {
        llvm::outs() << "Executable generated: " << exeFileName << "\n\n";
    }

    const auto exeResult = executeCommand(exeFileName);

    return 0;
}
