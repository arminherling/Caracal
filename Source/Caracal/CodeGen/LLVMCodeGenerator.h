#pragma once

#include <Caracal/API.h>
#include <Caracal/IR/Instruction.h>
#include <Caracal/IR/Module.h>
#include <Caracal/IR/ValueRef.h>
#include <Caracal/Semantic/Type.h>

#include <memory>
#include <unordered_map>

// forward declare llvm types to keep headers clean and avoid linking llvm into tests
namespace llvm 
{
    class Module;
    class Value;
    class Function;
    class Type;
    class IRBuilderBase;
}

namespace Caracal
{
    class LLVMCodeGenerator
    {
    public:
        LLVMCodeGenerator(const Module& irModule, llvm::Module& llvmModule);
        ~LLVMCodeGenerator();

        CARACAL_DELETE_COPY_DELETE_MOVE(LLVMCodeGenerator)

        [[nodiscard]] bool generate();

    private:
        [[nodiscard]] bool lowerGlobalConstant(const GlobalConstantDeclaration& globalConstant) noexcept;
        [[nodiscard]] bool lowerGlobalReference(const GlobalReferenceDeclaration& globalReference) noexcept;
        [[nodiscard]] bool lowerFunction(const Function& function) noexcept;
        [[nodiscard]] bool lowerInstruction(const Instruction& instruction) noexcept;
        [[nodiscard]] bool lowerTerminator(const Terminator& terminator) noexcept;
        [[nodiscard]] bool emitBinary(TemporaryId resultId, ValueRef leftRef, ValueRef rightRef, InstructionKind kind) noexcept;

        [[nodiscard]] llvm::Type* lowerType(Type type) const noexcept;
        [[nodiscard]] llvm::Value* lowerConstant(const ConstantValue& value) noexcept;
        [[nodiscard]] llvm::Value* tryResolve(ValueRef value) const noexcept;
        void defineValue(TemporaryId id, llvm::Value* value) noexcept;

        const Module& m_irModule;
        llvm::Module& m_llvmModule;
        std::unique_ptr<llvm::IRBuilderBase> m_irBuilder;
        llvm::Function* m_currentFunction;
        std::unordered_map<TemporaryId, llvm::Value*> m_values;
    };
}
