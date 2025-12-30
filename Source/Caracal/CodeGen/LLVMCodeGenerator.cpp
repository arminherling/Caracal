#include <Caracal/CodeGen/LLVMCodeGenerator.h>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Verifier.h>

namespace Caracal
{
    bool generateLLVMModule(const ParseTree& parseTree, llvm::Module& module) noexcept
    {
        auto& context = module.getContext();
        llvm::IRBuilder<> builder(context);

        // create main function declaration
        auto mainFunction = module.getFunction("main");
        if (mainFunction == nullptr)
        {
            auto mainFunctionType = llvm::FunctionType::get(builder.getInt32Ty(), false);
            mainFunction = llvm::Function::Create(mainFunctionType, llvm::Function::ExternalLinkage, "main", &module);
            verifyFunction(*mainFunction);
        }

        //int main()
        //{
        //   return 42;
        //}
        auto mainEntry = llvm::BasicBlock::Create(context, "entry", mainFunction);
        builder.SetInsertPoint(mainEntry);
        auto value = builder.getInt32(42);
        builder.CreateRet(value);

        return true;
    }
}
