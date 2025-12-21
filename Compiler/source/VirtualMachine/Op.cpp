#include "Op.h"

QString stringifyOp(Op op)
{
    switch (op)
    {
        case Op::LoadBool:
            return QString("LoadBool");
        case Op::NotBool:
            return QString("NotBool");
        case Op::EqualBool:
            return QString("EqualBool");
        case Op::NotEqualBool:
            return QString("NotEqualBool");
        case Op::LoadInt32:
            return QString("LoadInt32");
        case Op::AddInt32:
            return QString("AddInt32");
        case Op::SubtractInt32:
            return QString("SubtractInt32");
        case Op::MultiplyInt32:
            return QString("MultiplyInt32");
        case Op::DivideInt32:
            return QString("DivideInt32");
        case Op::NegateInt32:
            return QString("NegateInt32");
        case Op::EqualInt32:
            return QString("EqualInt32");
        case Op::NotEqualInt32:
            return QString("NotEqualInt32");
        case Op::GreaterInt32:
            return QString("GreaterInt32");
        case Op::GreaterOrEqualInt32:
            return QString("GreaterOrEqualInt32");
        case Op::LessInt32:
            return QString("LessInt32");
        case Op::LessOrEqualInt32:
            return QString("LessOrEqualInt32");
        case Op::FunctionCall:
            return QString("FunctionCall");
        case Op::Jump:
            return QString("Jump");
        case Op::JumpIfTrue:
            return QString("JumpIfTrue");
        case Op::JumpIfFalse:
            return QString("JumpIfFalse");
        case Op::Move:
            return QString("Move");
        case Op::PrintBool:
            return QString("PrintBool");
        case Op::PrintInt32:
            return QString("PrintInt32");
        case Op::PrintNewLine:
            return QString("PrintNewLine");
        case Op::Halt:
            return QString("Halt");
        default:
            TODO("String for Op code value was not defined yet");
    }
    return QString();
}
