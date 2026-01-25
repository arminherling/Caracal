#include <Caracal/API.h>
#include <Caracal/Syntax/ParseTree.h>
#include <Caracal/Semantic/TypeDatabase.h>
#include <filesystem>

namespace Caracal
{
    CARACAL_API int compileFile(std::filesystem::path filePath);

    CARACAL_API std::pair<bool, std::string> generateLLVMIR(ParseTree& parseTree, TypeDatabase& typeDatabase);
};
