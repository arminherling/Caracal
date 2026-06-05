#include <Caracal/API.h>
#include <Caracal/Diagnostics/DiagnosticOptions.h>
#include <Caracal/Syntax/ParseTree.h>
#include <Caracal/Semantic/SemanticContext.h>
#include <filesystem>
#include <string>

namespace Caracal
{
    CARACAL_API int compileFile(
        const std::filesystem::path& filePath,
        const DiagnosticOptions& diagnosticOptions = {});

    CARACAL_API std::pair<bool, std::string> generateIRText(SemanticContext& semanticContext);
    CARACAL_API std::pair<bool, std::string> generateLLVMIR(ParseTree& parseTree, SemanticContext& semanticContext, std::string targetTriple);
};
