#include "Op.h"

namespace Caracal
{
    std::string stringify(Op op)
    {
        switch (op)
        {
            case Op::LoadBool:
                return std::string("LoadBool");
            case Op::NotBool:
                return std::string("NotBool");
            case Op::EqualBool:
                return std::string("EqualBool");
            case Op::NotEqualBool:
                return std::string("NotEqualBool");
            case Op::LoadInt32:
                return std::string("LoadInt32");
            case Op::AddInt32:
                return std::string("AddInt32");
            case Op::SubtractInt32:
                return std::string("SubtractInt32");
            case Op::MultiplyInt32:
                return std::string("MultiplyInt32");
            case Op::DivideInt32:
                return std::string("DivideInt32");
            case Op::NegateInt32:
                return std::string("NegateInt32");
            case Op::EqualInt32:
                return std::string("EqualInt32");
            case Op::NotEqualInt32:
                return std::string("NotEqualInt32");
            case Op::GreaterInt32:
                return std::string("GreaterInt32");
            case Op::GreaterOrEqualInt32:
                return std::string("GreaterOrEqualInt32");
            case Op::LessInt32:
                return std::string("LessInt32");
            case Op::LessOrEqualInt32:
                return std::string("LessOrEqualInt32");
            case Op::FunctionCall:
                return std::string("FunctionCall");
            case Op::Jump:
                return std::string("Jump");
            case Op::JumpIfTrue:
                return std::string("JumpIfTrue");
            case Op::JumpIfFalse:
                return std::string("JumpIfFalse");
            case Op::Move:
                return std::string("Move");
            case Op::PrintBool:
                return std::string("PrintBool");
            case Op::PrintInt32:
                return std::string("PrintInt32");
            case Op::PrintNewLine:
                return std::string("PrintNewLine");
            case Op::Halt:
                return std::string("Halt");
            default:
                TODO("String for Op code value was not defined yet");
        }
        return std::string();
    }
}
