#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/ParseTree.h>
#include <llvm/IR/Module.h>

namespace Caracal
{
    CARACAL_API bool generateLLVMModule(const ParseTree& parseTree, llvm::Module& module) noexcept;
}
