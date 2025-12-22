#pragma once

#include <Defines.h>
#include <Compiler/API.h>
#include <VirtualMachine/ByteCode.h>
#include <VirtualMachine/CallFrame.h>
#include <VirtualMachine/Register.h>
#include <VirtualMachine/Value.h>

#include <vector>
#include <stack>

namespace Caracal
{
    class COMPILER_API VM
    {
    public:
        VM();

        i32 run(ByteCode& instructions);
        Value getValue(Register);
        Value getRelativeValue(Register);

    private:
        void setValue(Register, const Value&);
        void setRelativeValue(Register, const Value&);

        std::vector<Value> m_registers;
        std::stack<CallFrame> m_callFrames;
    };
}
