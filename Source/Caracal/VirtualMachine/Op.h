#pragma once

#include <Caracal/Defines.h>
#include <Caracal/API.h>
#include <string>

namespace Caracal
{
    enum Op : u8
    {
        LoadBool,
        NotBool,
        EqualBool,
        NotEqualBool,
        LoadInt32,
        AddInt32,
        SubtractInt32,
        MultiplyInt32,
        DivideInt32,
        NegateInt32,
        EqualInt32,
        NotEqualInt32,
        GreaterInt32,
        GreaterOrEqualInt32,
        LessInt32,
        LessOrEqualInt32,
        FunctionCall,
        Jump,
        JumpIfTrue,
        JumpIfFalse,
        Move,
        PrintBool,
        PrintInt32,
        PrintNewLine,
        Halt
    };

    CARACAL_API [[nodiscard]] std::string stringify(Op op);
}
