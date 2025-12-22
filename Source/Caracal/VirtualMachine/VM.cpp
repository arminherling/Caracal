#include "VM.h"

#include <Caracal/VirtualMachine/Op.h>

#include <iostream>
namespace Caracal
{
    VM::VM()
    {
        m_registers.resize(2048);
        m_callFrames.push({ .returnAddress = 0, .baseRegister = 0 });
    }

    void VM::setValue(Register reg, const Value& value)
    {
        m_registers[reg.index] = value;
    }

    void VM::setRelativeValue(Register reg, const Value& value)
    {
        auto& currentFrame = m_callFrames.top();

        m_registers[currentFrame.baseRegister + reg.index] = value;
    }

    Value VM::getValue(Register reg)
    {
        return m_registers[reg.index];
    }

    Value VM::getRelativeValue(Register reg)
    {
        auto currentFrame = m_callFrames.top();

        return m_registers[currentFrame.baseRegister + reg.index];
    }

    i32 VM::run(ByteCode& code)
    {
        u16 ip = 0;

        while (true)
        {
            auto op = static_cast<Op>(code.readUInt8(ip));
            switch (op)
            {
                case Op::LoadBool:
                {
                    auto reg = code.readUInt16(ip);
                    auto value = (bool)code.readUInt8(ip);

                    setRelativeValue(reg, value);

                    break;
                }
                case Op::NotBool:
                {
                    auto result = code.readUInt16(ip);
                    auto value = code.readUInt16(ip);

                    auto valueValue = getRelativeValue(value);

                    auto resultValue = Value(!valueValue.asBool());
                    setRelativeValue(result, resultValue);

                    break;
                }
                case Op::EqualBool:
                {
                    auto result = code.readUInt16(ip);
                    auto rhs = code.readUInt16(ip);
                    auto lhs = code.readUInt16(ip);

                    auto rhsValue = getRelativeValue(rhs);
                    auto lhsValue = getRelativeValue(lhs);

                    auto resultValue = Value(rhsValue.asBool() == lhsValue.asBool());
                    setRelativeValue(result, resultValue);

                    break;
                }
                case Op::NotEqualBool:
                {
                    auto result = code.readUInt16(ip);
                    auto rhs = code.readUInt16(ip);
                    auto lhs = code.readUInt16(ip);

                    auto rhsValue = getRelativeValue(rhs);
                    auto lhsValue = getRelativeValue(lhs);

                    auto resultValue = Value(rhsValue.asBool() != lhsValue.asBool());
                    setRelativeValue(result, resultValue);

                    break;
                }
                case Op::LoadInt32:
                {
                    auto reg = code.readUInt16(ip);
                    auto value = code.readInt32(ip);

                    setRelativeValue(reg, value);

                    break;
                }
                case Op::AddInt32:
                {
                    auto result = code.readUInt16(ip);
                    auto rhs = code.readUInt16(ip);
                    auto lhs = code.readUInt16(ip);

                    auto rhsValue = getRelativeValue(rhs);
                    auto lhsValue = getRelativeValue(lhs);

                    auto resultValue = Value(rhsValue.asInt32() + lhsValue.asInt32());
                    setRelativeValue(result, resultValue);

                    break;
                }
                case Op::SubtractInt32:
                {
                    auto result = code.readUInt16(ip);
                    auto rhs = code.readUInt16(ip);
                    auto lhs = code.readUInt16(ip);

                    auto rhsValue = getRelativeValue(rhs);
                    auto lhsValue = getRelativeValue(lhs);

                    auto resultValue = Value(rhsValue.asInt32() - lhsValue.asInt32());
                    setRelativeValue(result, resultValue);

                    break;
                }
                case Op::MultiplyInt32:
                {
                    auto result = code.readUInt16(ip);
                    auto rhs = code.readUInt16(ip);
                    auto lhs = code.readUInt16(ip);

                    auto rhsValue = getRelativeValue(rhs);
                    auto lhsValue = getRelativeValue(lhs);

                    auto resultValue = Value(rhsValue.asInt32() * lhsValue.asInt32());
                    setRelativeValue(result, resultValue);

                    break;
                }
                case Op::DivideInt32:
                {
                    auto result = code.readUInt16(ip);
                    auto rhs = code.readUInt16(ip);
                    auto lhs = code.readUInt16(ip);

                    auto rhsValue = getRelativeValue(rhs);
                    auto lhsValue = getRelativeValue(lhs);

                    auto resultValue = Value(rhsValue.asInt32() / lhsValue.asInt32());
                    setRelativeValue(result, resultValue);

                    break;
                }
                case Op::NegateInt32:
                {
                    auto result = code.readUInt16(ip);
                    auto value = code.readUInt16(ip);

                    auto valueValue = getRelativeValue(value);

                    auto resultValue = Value(-valueValue.asInt32());
                    setRelativeValue(result, resultValue);

                    break;
                }
                case Op::EqualInt32:
                {
                    auto result = code.readUInt16(ip);
                    auto rhs = code.readUInt16(ip);
                    auto lhs = code.readUInt16(ip);

                    auto rhsValue = getRelativeValue(rhs);
                    auto lhsValue = getRelativeValue(lhs);

                    auto resultValue = Value(rhsValue.asInt32() == lhsValue.asInt32());
                    setRelativeValue(result, resultValue);

                    break;
                }
                case Op::NotEqualInt32:
                {
                    auto result = code.readUInt16(ip);
                    auto rhs = code.readUInt16(ip);
                    auto lhs = code.readUInt16(ip);

                    auto rhsValue = getRelativeValue(rhs);
                    auto lhsValue = getRelativeValue(lhs);

                    auto resultValue = Value(rhsValue.asInt32() != lhsValue.asInt32());
                    setRelativeValue(result, resultValue);

                    break;
                }
                case Op::GreaterInt32:
                {
                    auto result = code.readUInt16(ip);
                    auto rhs = code.readUInt16(ip);
                    auto lhs = code.readUInt16(ip);

                    auto rhsValue = getRelativeValue(rhs);
                    auto lhsValue = getRelativeValue(lhs);

                    auto resultValue = Value(rhsValue.asInt32() > lhsValue.asInt32());
                    setRelativeValue(result, resultValue);

                    break;
                }
                case Op::GreaterOrEqualInt32:
                {
                    auto result = code.readUInt16(ip);
                    auto rhs = code.readUInt16(ip);
                    auto lhs = code.readUInt16(ip);

                    auto rhsValue = getRelativeValue(rhs);
                    auto lhsValue = getRelativeValue(lhs);

                    auto resultValue = Value(rhsValue.asInt32() >= lhsValue.asInt32());
                    setRelativeValue(result, resultValue);

                    break;
                }
                case Op::LessInt32:
                {
                    auto result = code.readUInt16(ip);
                    auto rhs = code.readUInt16(ip);
                    auto lhs = code.readUInt16(ip);

                    auto rhsValue = getRelativeValue(rhs);
                    auto lhsValue = getRelativeValue(lhs);

                    auto resultValue = Value(rhsValue.asInt32() < lhsValue.asInt32());
                    setRelativeValue(result, resultValue);

                    break;
                }
                case Op::LessOrEqualInt32:
                {
                    auto result = code.readUInt16(ip);
                    auto rhs = code.readUInt16(ip);
                    auto lhs = code.readUInt16(ip);

                    auto rhsValue = getRelativeValue(rhs);
                    auto lhsValue = getRelativeValue(lhs);

                    auto resultValue = Value(rhsValue.asInt32() <= lhsValue.asInt32());
                    setRelativeValue(result, resultValue);

                    break;
                }
                case Op::FunctionCall:
                {
                    auto functionEntryPoint = code.readUInt16(ip);
                    auto targetRegister = code.readUInt16(ip);

                    auto& currentFrame = m_callFrames.top();
                    auto functionBaseRegister = u16(currentFrame.baseRegister + targetRegister);

                    m_callFrames.push({ .returnAddress = ip, .baseRegister = functionBaseRegister });
                    ip = functionEntryPoint;

                    break;
                }
                case Op::Jump:
                {
                    auto target = code.readUInt16(ip);

                    ip = target;

                    break;
                }
                case Op::JumpIfTrue:
                {
                    auto condition = code.readUInt16(ip);
                    auto target = code.readUInt16(ip);

                    auto conditionValue = getRelativeValue(condition);
                    if (conditionValue.asBool())
                        ip = target;

                    break;
                }
                case Op::JumpIfFalse:
                {
                    auto condition = code.readUInt16(ip);
                    auto target = code.readUInt16(ip);

                    auto conditionValue = getRelativeValue(condition);
                    if (!conditionValue.asBool())
                        ip = target;

                    break;
                }
                case Op::Move:
                {
                    auto target = code.readUInt16(ip);
                    auto source = code.readUInt16(ip);

                    auto value = getRelativeValue(source);
                    setRelativeValue(target, value);

                    break;
                }
                case Op::PrintBool:
                {
                    auto reg = code.readUInt16(ip);
                    auto value = getRelativeValue(reg);

                    std::cout << std::boolalpha << value.asBool();

                    break;
                }
                case Op::PrintInt32:
                {
                    auto reg = code.readUInt16(ip);
                    auto value = getRelativeValue(reg);

                    std::cout << value.asInt32();

                    break;
                }
                case Op::PrintNewLine:
                {
                    std::cout << '\n';

                    break;
                }
                case Op::Halt:
                {
                    auto currentFrame = m_callFrames.top();
                    ip = currentFrame.returnAddress;

                    m_callFrames.pop();

                    if (m_callFrames.empty())
                        return 0;

                    break;
                }
                default:
                    TODO("Operation was was not defined yet");
            }
        }

        return 0;
    }
}
