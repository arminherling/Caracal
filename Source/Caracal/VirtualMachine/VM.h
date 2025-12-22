#pragma once

#include <Caracal/API.h>
#include <Caracal/Defines.h>
#include <Caracal/VirtualMachine/ByteCode.h>
#include <Caracal/VirtualMachine/CallFrame.h>
#include <Caracal/VirtualMachine/Register.h>
#include <Caracal/VirtualMachine/Value.h>

#include <vector>
#include <stack>

namespace Caracal
{
    class CARACAL_API VM
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
