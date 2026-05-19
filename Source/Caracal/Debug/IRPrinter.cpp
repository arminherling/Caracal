#include <Caracal/Debug/IRPrinter.h>
#include <Caracal/IR/AddInstruction.h>
#include <Caracal/IR/BranchTerminator.h>
#include <Caracal/IR/ConstantInstruction.h>
#include <Caracal/IR/DivideInstruction.h>
#include <Caracal/IR/ExternFunction.h>
#include <Caracal/IR/Function.h>
#include <Caracal/IR/MultiplyInstruction.h>
#include <Caracal/IR/PhiInstruction.h>
#include <Caracal/IR/ReturnTerminator.h>
#include <Caracal/IR/ReturnValueTerminator.h>
#include <Caracal/IR/SubtractInstruction.h>
#include <Caracal/IR/JumpTerminator.h>

#include <charconv>
#include <string>
#include <type_traits>
#include <variant>

namespace Caracal
{
    static std::string FormatConstantValue(const ConstantValue& value)
    {
        return std::visit([](const auto& payload) -> std::string
            {
                using Payload = std::decay_t<decltype(payload)>;

                if constexpr (std::is_same_v<Payload, bool>)
                {
                    if (payload)
                    {
                        return "true";
                    }
                    else
                    {
                        return "false";
                    }
                }

                if constexpr (std::is_same_v<Payload, u8> || std::is_same_v<Payload, i32>)
                    return std::to_string(payload);

                if constexpr (std::is_same_v<Payload, float>)
                {
                    char buffer[32]{};
                    const auto [end, error] = std::to_chars(buffer, buffer + sizeof(buffer), payload, std::chars_format::fixed, 6);
                    if (error != std::errc{})
                        return "<invalid-f32>";

                    auto text = std::string(buffer, end);
                    const auto dotIndex = text.find('.');
                    if (dotIndex == std::string::npos)
                    {
                        text += ".0";
                    }

                    return text;
                }

                return "<unknown-constant>";
            }, value.data());
    }

    IRPrinter::IRPrinter(const Module& module, i32 indentation)
        : m_module{ module }
        , m_builder{ indentation }
    {
    }

    std::string IRPrinter::prettyPrint()
    {
        bool hasPrintedFunction = false;

        const auto appendFunctionSeparator = [&]()
            {
                if (hasPrintedFunction)
                    m_builder.appendLine("");

                hasPrintedFunction = true;
            };

        for (const auto& function : m_module.externFunctions())
        {
            appendFunctionSeparator();
            prettyPrintExternFunction(function);
        }

        for (const auto& function : m_module.functions())
        {
            appendFunctionSeparator();
            prettyPrintFunction(function);
        }

        return m_builder.toString();
    }

    void IRPrinter::prettyPrintExternFunction(const ExternFunction& function)
    {
        m_builder
            .append("extern ")
            .append(function.name())
            .append("(");

        const auto& parameterTypes = function.parameterTypes();
        for (size_t index = 0; index < parameterTypes.size(); ++index)
        {
            if (index > 0)
                m_builder.append(", ");

            m_builder.append(formatType(parameterTypes[index]));
        }

        m_builder
            .append(") ")
            .append(formatType(function.returnType()))
            .appendLine("");
    }

    void IRPrinter::prettyPrintFunction(const Function& function)
    {
        m_builder
            .append("define ")
            .append(function.name())
            .append("(");

        const auto& parameterTypes = function.parameterTypes();
        for (size_t index = 0; index < parameterTypes.size(); ++index)
        {
            if (index > 0)
                m_builder.append(", ");

            m_builder.append(formatType(parameterTypes[index]));
        }

        m_builder
            .append(") ")
            .append(formatType(function.returnType()))
            .appendLine("");

        m_blockLabels.clear();
        for (const auto& block : function.blocks())
            m_blockLabels.emplace(block.id(), block.label());

        for (const auto& block : function.blocks())
            prettyPrintBlock(function, block);
    }

    void IRPrinter::prettyPrintBlock(const Function& function, const BasicBlock& block)
    {
        m_builder
            .append("block ")
            .append(block.label())
            .appendLine(":");
        m_builder.pushIndentation();

        for (const auto& instruction : block.instructions())
            prettyPrintInstruction(function, *instruction);

        if (block.hasTerminator())
            prettyPrintTerminator(function, *block.terminator());

        m_builder.popIndentation();
    }

    void IRPrinter::prettyPrintInstruction(const Function& function, const Instruction& instruction)
    {
        switch (instruction.kind())
        {
            case InstructionKind::Constant:
            {
                const auto& constant = static_cast<const ConstantInstruction&>(instruction);
                m_builder
                    .appendIndented(formatValue(ValueRef{ constant.resultId() }))
                    .append(" = const ")
                    .append(FormatConstantValue(constant.value()))
                    .append(" : ")
                    .append(formatType(constant.type()))
                    .appendLine("");
                break;
            }
            case InstructionKind::Add:
            {
                const auto& add = static_cast<const AddInstruction&>(instruction);
                m_builder
                    .appendIndented(formatValue(ValueRef{ add.resultId() }))
                    .append(" = add ")
                    .append(formatValue(add.leftValue()))
                    .append(", ")
                    .append(formatValue(add.rightValue()))
                    .append(" : ")
                    .append(formatType(add.type()))
                    .appendLine("");
                break;
            }
            case InstructionKind::Subtract:
            {
                const auto& subtract = static_cast<const SubtractInstruction&>(instruction);
                m_builder
                    .appendIndented(formatValue(ValueRef{ subtract.resultId() }))
                    .append(" = sub ")
                    .append(formatValue(subtract.leftValue()))
                    .append(", ")
                    .append(formatValue(subtract.rightValue()))
                    .append(" : ")
                    .append(formatType(subtract.type()))
                    .appendLine("");
                break;
            }
            case InstructionKind::Multiply:
            {
                const auto& multiply = static_cast<const MultiplyInstruction&>(instruction);
                m_builder
                    .appendIndented(formatValue(ValueRef{ multiply.resultId() }))
                    .append(" = mul ")
                    .append(formatValue(multiply.leftValue()))
                    .append(", ")
                    .append(formatValue(multiply.rightValue()))
                    .append(" : ")
                    .append(formatType(multiply.type()))
                    .appendLine("");
                break;
            }
            case InstructionKind::Divide:
            {
                const auto& divide = static_cast<const DivideInstruction&>(instruction);
                m_builder
                    .appendIndented(formatValue(ValueRef{ divide.resultId() }))
                    .append(" = div ")
                    .append(formatValue(divide.leftValue()))
                    .append(", ")
                    .append(formatValue(divide.rightValue()))
                    .append(" : ")
                    .append(formatType(divide.type()))
                    .appendLine("");
                break;
            }
            case InstructionKind::Phi:
            {
                const auto& phi = static_cast<const PhiInstruction&>(instruction);
                m_builder
                    .appendIndented(formatValue(ValueRef{ phi.resultId() }))
                    .append(" = phi ");

                const auto& phiInputs = phi.inputs();
                for (size_t index = 0; index < phiInputs.size(); ++index)
                {
                    if (index > 0)
                        m_builder.append(", ");

                    const auto& input = phiInputs[index];
                    m_builder
                        .append("[")
                        .append(blockLabel(function, input.blockId()))
                        .append(": ")
                        .append(formatValue(input.value()))
                        .append("]");
                }

                m_builder
                    .append(" : ")
                    .append(formatType(phi.type()))
                    .appendLine("");
                break;
            }
        }
    }

    void IRPrinter::prettyPrintTerminator(const Function& function, const Terminator& terminator)
    {
        switch (terminator.kind())
        {
            case TerminatorKind::Jump:
            {
                const auto& jump = static_cast<const JumpTerminator&>(terminator);
                m_builder
                    .appendIndented("jump ")
                    .append(blockLabel(function, jump.targetBlockId()))
                    .appendLine("");
                break;
            }
            case TerminatorKind::Branch:
            {
                const auto& branch = static_cast<const BranchTerminator&>(terminator);
                m_builder
                    .appendIndented("branch ")
                    .append(formatValue(branch.condition()))
                    .append(", ")
                    .append(blockLabel(function, branch.trueBlockId()))
                    .append(", ")
                    .append(blockLabel(function, branch.falseBlockId()))
                    .appendLine("");
                break;
            }
            case TerminatorKind::Return:
            {
                m_builder.appendIndentedLine("return");
                break;
            }
            case TerminatorKind::ReturnValue:
            {
                const auto& ret = static_cast<const ReturnValueTerminator&>(terminator);
                m_builder
                    .appendIndented("return ")
                    .append(formatValue(ret.value()))
                    .appendLine("");
                break;
            }
        }
    }

    std::string IRPrinter::formatType(Type type) const
    {
        const auto baseType = type.toBaseType();
        std::string name;

        if (baseType == Type::Void())
            name = "void";
        else if (baseType == Type::Bool())
            name = "bool";
        else if (baseType == Type::U8())
            name = "u8";
        else if (baseType == Type::I32())
            name = "i32";
        else if (baseType == Type::F32())
            name = "f32";
        else if (baseType == Type::String())
            name = "cstring";
        else if (baseType == Type::Discard())
            name = "discard";
        else if (baseType == Type::Undefined())
            name = "undefined";
        else if (baseType == Type::Function())
            name = "function";
        else if (baseType == Type::CVariadic())
            name = "...";
        else
            name = "type#" + std::to_string(baseType.id());

        if (type.isReference())
            name = "ref " + name;
        if (type.isOptional())
            name += "?";

        return name;
    }

    std::string IRPrinter::formatValue(ValueRef value) const
    {
        return "%" + std::to_string(value.id());
    }

    std::string_view IRPrinter::blockLabel(const Function& /*function*/, BlockId blockId) const
    {
        const auto result = m_blockLabels.find(blockId);
        if (result != m_blockLabels.end())
            return result->second;

        return "unknown";
    }
}
