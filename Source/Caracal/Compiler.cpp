#include <Caracal/Compiler.h>
#include <Caracal/Semantic/TypeCheckerOptions.h>
#include <Caracal/Semantic/TypeChecker.h>
#include <Caracal/Debug/IRPrinter.h>
#include <Caracal/Text/File.h>
#include <Caracal/Diagnostics/DiagnosticsBag.h>
#include <Caracal/Diagnostics/DiagnosticPrinter.h>
#include <Caracal/Syntax/Lexer.h>
#include <Caracal/Syntax/Parser.h>
#include <Caracal/CodeGen/LLVMCodeGenerator.h>
#include <Caracal/IR/IRLowerer.h>

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/Program.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <iostream>

namespace Caracal
{
    static int ExecuteCommand(const std::string& command)
    {
        return std::system(command.c_str());
    }

    static std::vector<std::filesystem::path> CollectCaraFiles(
        const std::filesystem::path& directoryPath) noexcept
    {
        std::vector<std::filesystem::path> caraFilePaths{};

        if (!std::filesystem::exists(directoryPath) || !std::filesystem::is_directory(directoryPath))
            return caraFilePaths;

        for (const auto& file : std::filesystem::recursive_directory_iterator(directoryPath))
        {
            if (file.is_regular_file() && file.path().extension() == ".cara")
                caraFilePaths.push_back(file.path());
        }

        return caraFilePaths;
    }

    static bool PopulateModule(SemanticContext& semanticContext, llvm::Module& llvmModule)
    {
        Module irModule{};
        IRLowerer lowerer{ semanticContext };
        if (!lowerer.lower(irModule))
            return false;

        LLVMCodeGenerator codeGenerator{ irModule, llvmModule };
        if (!codeGenerator.generate())
            return false;

        // catch malformed IR before it is emitted or snapshotted; verifyModule returns true when broken
        if (llvm::verifyModule(llvmModule, &llvm::errs()))
            return false;

        return true;
    }

    int compileFile(
        const std::filesystem::path& filePath,
        const DiagnosticOptions& diagnosticOptions)
    {
        const auto coreDirectoryPath = std::filesystem::path("Core");
        auto caraFiles = CollectCaraFiles(coreDirectoryPath);

        Caracal::DiagnosticsBag diagnostics;
        std::vector<Caracal::ParseTreeUPtr> parseTrees;
        for (const auto& caraFilePath : caraFiles)
        {
            auto content = Caracal::File::readText(caraFilePath);
            if (!content.has_value())
            {
                std::cout << "Failed to read file: " << caraFilePath << "\n";
                continue;
            }

            auto source = std::make_shared<Caracal::SourceText>(content.value(), caraFilePath);
            const auto diagnosticCount = diagnostics.diagnostics().size();
            const auto tokens = Caracal::lex(source, diagnostics);
            if (diagnostics.diagnostics().size() != diagnosticCount)
            {
                // skip parsing if there were lexing errors
                continue;
            }

            auto parseTree = Caracal::parse(tokens, diagnostics);
            parseTrees.push_back(std::move(parseTree));
        }

        auto fileContent = Caracal::File::readText(filePath);
        if (!fileContent.has_value())
        {
            std::cout << "Failed to read the file content.\n";
            return 1;
        }

        auto source = std::make_shared<Caracal::SourceText>(fileContent.value(), filePath);
        const auto diagnosticCount = diagnostics.diagnostics().size();
        const auto tokens = Caracal::lex(source, diagnostics);
        if (diagnostics.diagnostics().size() == diagnosticCount)
        {
            auto parseTree = Caracal::parse(tokens, diagnostics);
            parseTrees.push_back(std::move(parseTree));
        }

        if (diagnostics.hasErrors())
        {
            Caracal::writeDiagnostics(std::cout, diagnostics, diagnosticOptions);
            return 1;
        }

        Caracal::SemanticContext semanticContext = Caracal::SemanticContext::WithBuiltins();
        Caracal::TypeCheckerOptions options{
            .defaultIntegerType = Caracal::Type::I32(),
            .defaultFloatingType = Caracal::Type::F32(),
            .defaultEnumBaseType = Caracal::Type::U8()
        };

        auto wasSuccessful = Caracal::typeCheck(parseTrees, options, semanticContext, diagnostics);
        if (!wasSuccessful)
        {
            Caracal::writeDiagnostics(std::cout, diagnostics, diagnosticOptions);
            return 1;
        }

        if (!diagnostics.diagnostics().empty())
        {
            Caracal::writeDiagnostics(std::cout, diagnostics, diagnosticOptions);
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

        wasSuccessful = PopulateModule(semanticContext, *llvmModule);
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
            return 1;
        }
        else
        {
            llvm::outs() << "Executable generated: " << exeFileName << "\n\n";
        }

        const auto exeResult = ExecuteCommand(exeFileName);
        return exeResult;
    }

    std::pair<bool, std::string> generateIRText(SemanticContext& semanticContext)
    {
        Module module{};
        IRLowerer lowerer{ semanticContext };
        const auto wasSuccessful = lowerer.lower(module);
        if (!wasSuccessful)
            return std::make_pair(false, std::string{});

        IRPrinter printer{ module };
        return std::make_pair(true, printer.prettyPrint());
    }

    std::pair<bool, std::string> generateLLVMIRFromIR(SemanticContext& semanticContext, std::string targetTriple)
    {
        llvm::LLVMContext context;
        auto llvmModule = std::make_unique<llvm::Module>("CaracalModule", context);
        llvmModule->setTargetTriple(llvm::Triple(targetTriple));

        if (!PopulateModule(semanticContext, *llvmModule))
            return std::make_pair(false, std::string{});

        std::string irOutput;
        llvm::raw_string_ostream irStream(irOutput);
        llvmModule->print(irStream, nullptr, true, true);
        irStream.flush();

        return std::make_pair(true, irOutput);
    }
}
