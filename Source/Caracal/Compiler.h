#include <Caracal/API.h>
#include <Caracal/Syntax/ParseTree.h>
#include <Caracal/Semantic/Module.h>
#include <filesystem>

namespace Caracal
{
    CARACAL_API int compileFile(const std::filesystem::path& filePath);

    CARACAL_API std::pair<bool, std::string> generateLLVMIR(ParseTree& parseTree, Module& caracalModule);
};
