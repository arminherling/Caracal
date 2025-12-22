#pragma once

#include <Defines.h>
#include <Compiler/API.h>
#include <VirtualMachine/ByteCode.h>
#include <VirtualMachine/FunctionCallLocation.h>
#include <VirtualMachine/Register.h>
#include <VirtualMachine/Label.h>
#include <VirtualMachine/JumpIndex.h>

namespace Caracal
{
    class COMPILER_API ByteCodeAssembler
    {
    public:
        ByteCodeAssembler(ByteCode& byteCode);

        void emitLoadBool(Register reg, bool value);
        void emitNotBool(Register target, Register value);
        void emitEqualBool(Register target, Register rhs, Register lhs);
        void emitNotEqualBool(Register target, Register rhs, Register lhs);
        void emitLoadInt32(Register reg, i32 value);
        void emitAddInt32(Register target, Register rhs, Register lhs);
        void emitSubtractInt32(Register target, Register rhs, Register lhs);
        void emitMultiplyInt32(Register target, Register rhs, Register lhs);
        void emitDivideInt32(Register target, Register rhs, Register lhs);
        void emitNegateInt32(Register target, Register value);
        void emitEqualInt32(Register target, Register rhs, Register lhs);
        void emitNotEqualInt32(Register target, Register rhs, Register lhs);
        void emitGreaterInt32(Register target, Register rhs, Register lhs);
        void emitGreaterOrEqualInt32(Register target, Register rhs, Register lhs);
        void emitLessInt32(Register target, Register rhs, Register lhs);
        void emitLessOrEqualInt32(Register target, Register rhs, Register lhs);
        void emitMove(Register target, Register value);
        Label createLabel();
        FunctionDeclaration declareFunction(const std::string& name, u8 returnValues, u8 parameterValues);
        FunctionCallLocation emitFunctionCall(const std::string& name, Register resultTarget);
        JumpIndex emitJump();
        void emitJump(Label label);
        JumpIndex emitJumpIfTrue(Register value);
        void emitJumpIfTrue(Register value, Label label);
        JumpIndex emitJumpIfFalse(Register value);
        void emitJumpIfFalse(Register value, Label label);
        void patchJump(JumpIndex jump, Label label);
        void emitPrintBool(Register reg);
        void emitPrintInt32(Register reg);
        void emitPrintNewLine();
        void emitHalt();

        void patchFunctionCalls();

    private:
        ByteCode& m_byteCode;
        std::vector<FunctionCallLocation> functionCalls;

        JumpIndex writeJumpTarget();
    };
}
