#pragma once

#include <Caracal/API.h>
#include <Caracal/Defines.h>
#include <Caracal/IR/BasicBlock.h>
#include <Caracal/IR/ExternFunction.h>
#include <Caracal/IR/Function.h>
#include <Caracal/IR/LocalSlotRef.h>
#include <Caracal/IR/Module.h>
#include <Caracal/IR/Terminator.h>
#include <Caracal/IR/ValueRef.h>
#include <Caracal/Text/StringBuilder.h>

#include <string>

namespace Caracal
{
    class CARACAL_API IRPrinter
    {
    public:
        explicit IRPrinter(const Module& module, i32 indentation = 4);

        CARACAL_DELETE_COPY_DELETE_MOVE(IRPrinter)

        [[nodiscard]] std::string prettyPrint();

    private:
        void prettyPrintEnumDeclaration(const EnumDeclaration& enumDeclaration);
        void prettyPrintTypeDeclaration(const TypeDeclaration& typeDeclaration);
        void prettyPrintExternFunction(const ExternFunction& function);
        void prettyPrintFunction(const Function& function);
        void prettyPrintBlock(const Function& function, const BasicBlock& block);
        void prettyPrintInstruction(const Function& function, const Instruction& instruction);
        void prettyPrintTerminator(const Function& function, const Terminator& terminator);

        void appendType(Type type);
        void appendSlot(LocalSlotRef slot);
        void appendValue(ValueRef value);
        void appendBlockLabel(const Function& function, BlockId blockId);

    private:
        const Module& m_module;
        StringBuilder m_builder;
    };
}
