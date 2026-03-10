#include <Caracal/Compiler.h>
#include <Caracal/Semantic/TypeCheckerOptions.h>
#include <Caracal/Semantic/TypeChecker.h>
#include <Caracal/Text/File.h>
#include <Caracal/Debug/DiagnosticsBag.h>
#include <Caracal/Syntax/Lexer.h>
#include <Caracal/Syntax/Parser.h>
#include <Caracal/CodeGen/LLVMCodeGenerator.h>

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Support/Program.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <iostream>

namespace Caracal
{
    static int executeCommand(const std::string& command)
    {
        return std::system(command.c_str());
    }

    int compileFile(std::filesystem::path filePath)
    {
        auto fileContent = Caracal::File::readText(filePath);
        if (!fileContent.has_value())
        {
            std::cout << "Failed to read the file content.\n";
            return 1;
        }

        auto source = std::make_shared<Caracal::SourceText>(fileContent.value());
        Caracal::DiagnosticsBag diagnostics;

        auto tokens = Caracal::lex(source, diagnostics);
        auto parseTree = Caracal::parse(tokens, diagnostics);
        std::vector<Caracal::ParseTreeUPtr> parseTrees;
        parseTrees.push_back(std::move(parseTree));

        if (!diagnostics.Diagnostics().empty())
        {
            std::cout << "Errors found during parsing!";
            return 1;
        }

        Caracal::Module caracalModule = Caracal::Module::WithBuiltins();
        Caracal::TypeCheckerOptions options{
            .defaultIntegerType = Caracal::Type::I32(),
            .defaultFloatingType = Caracal::Type::F32(),
            .defaultEnumBaseType = Caracal::Type::U8()
        };

        auto wasSuccessful = Caracal::typeCheck(parseTrees, options, caracalModule, diagnostics);
        if (!wasSuccessful)
        {
            std::cout << "Type checking failed!";
            return 1;
        }

        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();

        llvm::LLVMContext context;
        auto llvmModule = std::make_unique<llvm::Module>("CaracalModule", context);

        auto defaultTargetTriple = llvm::sys::getDefaultTargetTriple();
        llvm::outs() << "TargetTriple: " << defaultTargetTriple << '\n';
        std::string targetError;
        auto target = llvm::TargetRegistry::lookupTarget(defaultTargetTriple, targetError);
        auto triple = llvm::Triple(defaultTargetTriple);
        if (!target)
        {
            llvm::errs() << "Error: " << targetError << "\n";
            return 1;
        }

        auto CPU = "generic";
        auto features = "";
        llvm::TargetOptions targetOptions{};
        auto relocModel = std::optional<llvm::Reloc::Model>();
        auto targetMachine = target->createTargetMachine(triple, CPU, features, targetOptions, relocModel);
        llvmModule->setDataLayout(targetMachine->createDataLayout());
        llvmModule->setTargetTriple(triple);

        wasSuccessful = Caracal::generateLLVMModule(caracalModule, *llvmModule);
        if (!wasSuccessful)
        {
            std::cout << "Module not generated!";
            return 1;
        }

        std::string irOutput;
        llvm::raw_string_ostream irStream(irOutput);
        llvmModule->print(irStream, nullptr);
        irStream.flush();
        llvm::outs() << "Module IR:\n" << irOutput << "\n";

        const auto inputFileName = filePath.stem().string();
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
        passManager.run(*llvmModule);
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
        return exeResult;
    }

    std::pair<bool, std::string> generateLLVMIR(ParseTree& parseTree, Module& caracalModule)
    {
        llvm::InitializeNativeTarget();
        llvm::InitializeNativeTargetAsmPrinter();

        llvm::LLVMContext context;
        auto module = std::make_unique<llvm::Module>("CaracalModule", context);

        auto defaultTargetTriple = llvm::sys::getDefaultTargetTriple();
        llvm::outs() << "TargetTriple: " << defaultTargetTriple << '\n';
        std::string targetError;
        auto target = llvm::TargetRegistry::lookupTarget(defaultTargetTriple, targetError);
        auto triple = llvm::Triple(defaultTargetTriple);
        if (!target)
        {
            llvm::errs() << "Error: " << targetError << "\n";
            return std::make_pair(false, "");
        }

        auto CPU = "generic";
        auto features = "";
        llvm::TargetOptions targetOptions{};
        auto relocModel = std::optional<llvm::Reloc::Model>();
        auto targetMachine = target->createTargetMachine(triple, CPU, features, targetOptions, relocModel);
        module->setDataLayout(targetMachine->createDataLayout());
        module->setTargetTriple(triple);

        auto wasSuccessful = Caracal::generateLLVMModule(caracalModule, *module);
        if (!wasSuccessful)
        {
            std::cout << "Module not generated!";
            return std::make_pair(false, "");
        }

        std::string irOutput;
        llvm::raw_string_ostream irStream(irOutput);
        module->print(irStream, nullptr);
        irStream.flush();

        return std::make_pair(true, irOutput);
    }
}
