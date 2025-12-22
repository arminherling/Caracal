#include "ByteCodeDisassembler.h"
//
//#include <VirtualMachine/Op.h>
//
//namespace
//{
//    const auto LongestOpNameSize = stringify(Op::GreaterOrEqualInt32).size();
//
//    QString StringifyAndPadOp(Op op)
//    {
//        return stringifyOp(op).leftJustified(LongestOpNameSize, ' ');
//    }
//
//    QString StringifyRegister(u16 reg)
//    {
//        return QString("$%1").arg(reg);
//    }
//
//    QString StringifyAddress(u16 address)
//    {
//        return QString("@%1").arg(QString::number(address).rightJustified(6, '0'));
//    }
//
//    QString StringifyBool(bool value)
//    {
//        return QString(value ? "true" : "false");
//    }
//}
//
//ByteCodeDisassembler::ByteCodeDisassembler(ByteCode& byteCode)
//    : m_byteCode{ byteCode }
//    , m_ip{ 0 }
//    , m_output{ QString() }
//    , m_stream{ &m_output }
//{
//}
//
//QString ByteCodeDisassembler::PrettyPrint()
//{
//    while (m_ip < m_byteCode.data.size())
//    {
//        PrettyPrintOp();
//    }
//
//    return toUtf8();
//}
//
//void ByteCodeDisassembler::PrettyPrintOp()
//{
//    stream() << QString::number(m_ip).rightJustified(6, '0') << ' ';
//
//    auto op = static_cast<Op>(m_byteCode.readUInt8(m_ip));
//    switch (op)
//    {
//        case Op::LoadBool:
//        {
//            PrettyPrintLoadBool();
//            break;
//        }
//        case Op::NotBool:
//        {
//            PrettyPrintNotBool();
//            break;
//        }
//        case Op::EqualBool:
//        {
//            PrettyPrintEqualBool();
//            break;
//        }
//        case Op::NotEqualBool:
//        {
//            PrettyPrintNotEqualBool();
//            break;
//        }
//        case Op::LoadInt32:
//        {
//            PrettyPrintLoadInt32();
//            break;
//        }
//        case Op::AddInt32:
//        {
//            PrettyPrintAddInt32();
//            break;
//        }
//        case Op::SubtractInt32:
//        {
//            PrettyPrintSubtractInt32();
//            break;
//        }
//        case Op::MultiplyInt32:
//        {
//            PrettyPrintMultiplyInt32();
//            break;
//        }
//        case Op::DivideInt32:
//        {
//            PrettyPrintDivideInt32();
//            break;
//        }
//        case Op::NegateInt32:
//        {
//            PrettyPrintNegateInt32();
//            break;
//        }
//        case Op::EqualInt32:
//        {
//            PrettyPrintEqualInt32();
//            break;
//        }
//        case Op::NotEqualInt32:
//        {
//            PrettyPrintNotEqualInt32();
//            break;
//        }
//        case Op::GreaterInt32:
//        {
//            PrettyPrintGreaterInt32();
//            break;
//        }
//        case Op::GreaterOrEqualInt32:
//        {
//            PrettyPrintGreaterOrEqualInt32();
//            break;
//        }
//        case Op::LessInt32:
//        {
//            PrettyPrintLessInt32();
//            break;
//        }
//        case Op::LessOrEqualInt32:
//        {
//            PrettyPrintLessOrEqualInt32();
//            break;
//        }
//        case Op::FunctionCall:
//        {
//            PrettyPrintFunctionCall();
//            break;
//        }
//        case Op::Jump:
//        {
//            PrettyPrintJump();
//            break;
//        }
//        case Op::JumpIfTrue:
//        {
//            PrettyPrintJumpIfTrue();
//            break;
//        }
//        case Op::JumpIfFalse:
//        {
//            PrettyPrintJumpIfFalse();
//            break;
//        }
//        case Op::Move: 
//        {
//            PrettyPrintMove();
//            break;
//        }
//        case Op::PrintBool:
//        {
//            PrettyPrintPrintBool();
//            break;
//        }
//        case Op::PrintInt32:
//        {
//            PrettyPrintPrintInt32();
//            break;
//        }
//        case Op::PrintNewLine:
//        {
//            PrettyPrintPrintNewLine();
//            break;
//        }
//        case Op::Halt:
//        {
//            PrettyPrintHalt();
//            break;
//        }
//        default:
//            TODO("Missing Op!!");
//    }
//}
//
//void ByteCodeDisassembler::PrettyPrintLoadBool()
//{
//    auto reg = m_byteCode.readUInt16(m_ip);
//    auto value = (bool)m_byteCode.readUInt8(m_ip);
//
//    stream() << StringifyAndPadOp(Op::LoadBool) << ' ' << StringifyRegister(reg) << ' ' << StringifyBool(value) << NewLine();
//}
//
//void ByteCodeDisassembler::PrettyPrintNotBool()
//{
//    auto result = m_byteCode.readUInt16(m_ip);
//    auto value = m_byteCode.readUInt16(m_ip);
//
//    stream() << StringifyAndPadOp(Op::NotBool) << ' ' << StringifyRegister(result) << ' ' << StringifyRegister(value) << NewLine();
//}
//
//void ByteCodeDisassembler::PrettyPrintEqualBool()
//{
//    auto result = m_byteCode.readUInt16(m_ip);
//    auto rhs = m_byteCode.readUInt16(m_ip);
//    auto lhs = m_byteCode.readUInt16(m_ip);
//
//    stream() << StringifyAndPadOp(Op::EqualBool) << ' ' << StringifyRegister(result) << ' ' << StringifyRegister(rhs) << ' ' << StringifyRegister(lhs) << NewLine();
//}
//
//void ByteCodeDisassembler::PrettyPrintNotEqualBool()
//{
//    auto result = m_byteCode.readUInt16(m_ip);
//    auto rhs = m_byteCode.readUInt16(m_ip);
//    auto lhs = m_byteCode.readUInt16(m_ip);
//
//    stream() << StringifyAndPadOp(Op::NotEqualBool) << ' ' << StringifyRegister(result) << ' ' << StringifyRegister(rhs) << ' ' << StringifyRegister(lhs) << NewLine();
//}
//
//void ByteCodeDisassembler::PrettyPrintLoadInt32()
//{
//    auto reg = m_byteCode.readUInt16(m_ip);
//    auto value = m_byteCode.readInt32(m_ip);
//
//    stream() << StringifyAndPadOp(Op::LoadInt32) << ' ' << StringifyRegister(reg) << ' ' << QString::number(value) << NewLine();
//}
//
//void ByteCodeDisassembler::PrettyPrintAddInt32()
//{
//    auto result = m_byteCode.readUInt16(m_ip);
//    auto rhs = m_byteCode.readUInt16(m_ip);
//    auto lhs = m_byteCode.readUInt16(m_ip);
//
//    stream() << StringifyAndPadOp(Op::AddInt32) << ' ' << StringifyRegister(result) << ' ' << StringifyRegister(rhs) << ' ' << StringifyRegister(lhs) << NewLine();
//}
//
//void ByteCodeDisassembler::PrettyPrintSubtractInt32()
//{
//    auto result = m_byteCode.readUInt16(m_ip);
//    auto rhs = m_byteCode.readUInt16(m_ip);
//    auto lhs = m_byteCode.readUInt16(m_ip);
//
//    stream() << StringifyAndPadOp(Op::SubtractInt32) << ' ' << StringifyRegister(result) << ' ' << StringifyRegister(rhs) << ' ' << StringifyRegister(lhs) << NewLine();
//}
//
//void ByteCodeDisassembler::PrettyPrintMultiplyInt32()
//{
//    auto result = m_byteCode.readUInt16(m_ip);
//    auto rhs = m_byteCode.readUInt16(m_ip);
//    auto lhs = m_byteCode.readUInt16(m_ip);
//
//    stream() << StringifyAndPadOp(Op::MultiplyInt32) << ' ' << StringifyRegister(result) << ' ' << StringifyRegister(rhs) << ' ' << StringifyRegister(lhs) << NewLine();
//}
//
//void ByteCodeDisassembler::PrettyPrintDivideInt32()
//{
//    auto result = m_byteCode.readUInt16(m_ip);
//    auto rhs = m_byteCode.readUInt16(m_ip);
//    auto lhs = m_byteCode.readUInt16(m_ip);
//
//    stream() << StringifyAndPadOp(Op::DivideInt32) << ' ' << StringifyRegister(result) << ' ' << StringifyRegister(rhs) << ' ' << StringifyRegister(lhs) << NewLine();
//}
//
//void ByteCodeDisassembler::PrettyPrintNegateInt32()
//{
//    auto result = m_byteCode.readUInt16(m_ip);
//    auto value = m_byteCode.readUInt16(m_ip);
//
//    stream() << StringifyAndPadOp(Op::NegateInt32) << ' ' << StringifyRegister(result) << ' ' << StringifyRegister(value) << NewLine();
//}
//
//void ByteCodeDisassembler::PrettyPrintEqualInt32()
//{
//    auto result = m_byteCode.readUInt16(m_ip);
//    auto rhs = m_byteCode.readUInt16(m_ip);
//    auto lhs = m_byteCode.readUInt16(m_ip);
//
//    stream() << StringifyAndPadOp(Op::EqualInt32) << ' ' << StringifyRegister(result) << ' ' << StringifyRegister(rhs) << ' ' << StringifyRegister(lhs) << NewLine();
//}
//
//void ByteCodeDisassembler::PrettyPrintNotEqualInt32()
//{
//    auto result = m_byteCode.readUInt16(m_ip);
//    auto rhs = m_byteCode.readUInt16(m_ip);
//    auto lhs = m_byteCode.readUInt16(m_ip);
//
//    stream() << StringifyAndPadOp(Op::NotEqualInt32) << ' ' << StringifyRegister(result) << ' ' << StringifyRegister(rhs) << ' ' << StringifyRegister(lhs) << NewLine();
//}
//
//void ByteCodeDisassembler::PrettyPrintGreaterInt32()
//{
//    auto result = m_byteCode.readUInt16(m_ip);
//    auto rhs = m_byteCode.readUInt16(m_ip);
//    auto lhs = m_byteCode.readUInt16(m_ip);
//
//    stream() << StringifyAndPadOp(Op::GreaterInt32) << ' ' << StringifyRegister(result) << ' ' << StringifyRegister(rhs) << ' ' << StringifyRegister(lhs) << NewLine();
//}
//
//void ByteCodeDisassembler::PrettyPrintGreaterOrEqualInt32()
//{
//    auto result = m_byteCode.readUInt16(m_ip);
//    auto rhs = m_byteCode.readUInt16(m_ip);
//    auto lhs = m_byteCode.readUInt16(m_ip);
//
//    stream() << StringifyAndPadOp(Op::GreaterOrEqualInt32) << ' ' << StringifyRegister(result) << ' ' << StringifyRegister(rhs) << ' ' << StringifyRegister(lhs) << NewLine();
//}
//
//void ByteCodeDisassembler::PrettyPrintLessInt32()
//{
//    auto result = m_byteCode.readUInt16(m_ip);
//    auto rhs = m_byteCode.readUInt16(m_ip);
//    auto lhs = m_byteCode.readUInt16(m_ip);
//
//    stream() << StringifyAndPadOp(Op::LessInt32) << ' ' << StringifyRegister(result) << ' ' << StringifyRegister(rhs) << ' ' << StringifyRegister(lhs) << NewLine();
//}
//
//void ByteCodeDisassembler::PrettyPrintLessOrEqualInt32()
//{
//    auto result = m_byteCode.readUInt16(m_ip);
//    auto rhs = m_byteCode.readUInt16(m_ip);
//    auto lhs = m_byteCode.readUInt16(m_ip);
//
//    stream() << StringifyAndPadOp(Op::LessOrEqualInt32) << ' ' << StringifyRegister(result) << ' ' << StringifyRegister(rhs) << ' ' << StringifyRegister(lhs) << NewLine();
//}
//
//void ByteCodeDisassembler::PrettyPrintFunctionCall()
//{
//    auto functionEntryPoint = m_byteCode.readUInt16(m_ip);
//    auto returnRegister = m_byteCode.readUInt16(m_ip);
//
//    stream() << StringifyAndPadOp(Op::FunctionCall) << ' ' << StringifyAddress(functionEntryPoint) << ' ' << StringifyRegister(returnRegister) << NewLine();
//}
//
//void ByteCodeDisassembler::PrettyPrintJump()
//{
//    auto target = m_byteCode.readUInt16(m_ip);
//
//    stream() << StringifyAndPadOp(Op::Jump) << ' ' << StringifyAddress(target) << NewLine();
//}
//
//void ByteCodeDisassembler::PrettyPrintJumpIfTrue()
//{
//    auto condition = m_byteCode.readUInt16(m_ip);
//    auto target = m_byteCode.readUInt16(m_ip);
//
//    stream() << StringifyAndPadOp(Op::JumpIfTrue) << ' ' << StringifyRegister(condition) << ' ' << StringifyAddress(target) << NewLine();
//}
//
//void ByteCodeDisassembler::PrettyPrintJumpIfFalse()
//{
//    auto condition = m_byteCode.readUInt16(m_ip);
//    auto target = m_byteCode.readUInt16(m_ip);
//
//    stream() << StringifyAndPadOp(Op::JumpIfFalse) << ' ' << StringifyRegister(condition) << ' ' << StringifyAddress(target) << NewLine();
//}
//
//void ByteCodeDisassembler::PrettyPrintMove()
//{
//    auto result = m_byteCode.readUInt16(m_ip);
//    auto value = m_byteCode.readUInt16(m_ip);
//
//    stream() << StringifyAndPadOp(Op::Move) << ' ' << StringifyRegister(result) << ' ' << StringifyRegister(value) << NewLine();
//}
//
//void ByteCodeDisassembler::PrettyPrintPrintBool()
//{
//    auto reg = m_byteCode.readUInt16(m_ip);
//
//    stream() << StringifyAndPadOp(Op::PrintBool) << ' ' << StringifyRegister(reg) << NewLine();
//}
//
//void ByteCodeDisassembler::PrettyPrintPrintInt32()
//{
//    auto reg = m_byteCode.readUInt16(m_ip);
//
//    stream() << StringifyAndPadOp(Op::PrintInt32) << ' ' << StringifyRegister(reg) << NewLine();
//}
//
//void ByteCodeDisassembler::PrettyPrintPrintNewLine()
//{
//    stream() << StringifyAndPadOp(Op::PrintNewLine) << NewLine();
//}
//
//void ByteCodeDisassembler::PrettyPrintHalt()
//{
//    stream() << StringifyAndPadOp(Op::Halt) << NewLine();
//}
