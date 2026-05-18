#include <Caracal/Debug/IRPrinter.h>
#include <Caracal/IR/BranchTerminator.h>
#include <Caracal/IR/ConstantInstruction.h>
#include <Caracal/IR/Function.h>
#include <Caracal/IR/PhiInstruction.h>
#include <Caracal/IR/ReturnTerminator.h>
#include <Caracal/IR/ReturnValueTerminator.h>
#include <Caracal/IR/JumpTerminator.h>

#include <charconv>
#include <sstream>
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
                    const auto [end, error] = std::to_chars(buffer, buffer + sizeof(buffer), payload);
                    if (error != std::errc{})
                        return "<invalid-f32>";

                    return std::string(buffer, end);
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
        bool isFirstFunction = true;
        for (const auto& function : m_module.functions())
        {
            if (!isFirstFunction)
                m_builder.appendLine("");

            prettyPrintFunction(function);
            isFirstFunction = false;
        }

        return m_builder.toString();
    }

    void IRPrinter::prettyPrintFunction(const Function& function)
    {
        std::ostringstream signature;
        signature << "def " << function.name() << "(";

        const auto& parameterTypes = function.parameterTypes();
        for (size_t index = 0; index < parameterTypes.size(); ++index)
        {
            if (index > 0)
                signature << ", ";

            signature << formatType(parameterTypes[index]);
        }

        signature << ") " << formatType(function.returnType());
        m_builder.appendLine(signature.str());

        m_blockLabels.clear();
        for (const auto& block : function.blocks())
            m_blockLabels.emplace(block.id(), block.label());

        for (const auto& block : function.blocks())
            prettyPrintBlock(function, block);
    }

    void IRPrinter::prettyPrintBlock(const Function& function, const BasicBlock& block)
    {
        m_builder.append("block ").append(block.label()).appendLine(":");
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
                std::ostringstream line;
                line << formatValue(ValueRef{ constant.resultId() })
                    << " = const " << FormatConstantValue(constant.value())
                    << " : " << formatType(constant.type());
                m_builder.appendIndentedLine(line.str());
                break;
            }
            case InstructionKind::Phi:
            {
                const auto& phi = static_cast<const PhiInstruction&>(instruction);
                std::ostringstream line;
                line << formatValue(ValueRef{ phi.resultId() }) << " = phi ";

                const auto& phiInputs = phi.inputs();
                for (size_t index = 0; index < phiInputs.size(); ++index)
                {
                    if (index > 0)
                        line << ", ";

                    const auto& input = phiInputs[index];
                    line << "[" << blockLabel(function, input.blockId()) << ": " << formatValue(input.value()) << "]";
                }

                line << " : " << formatType(phi.type());
                m_builder.appendIndentedLine(line.str());
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
                std::ostringstream line;
                line << "jump " << blockLabel(function, jump.targetBlockId());
                m_builder.appendIndentedLine(line.str());
                break;
            }
            case TerminatorKind::Branch:
            {
                const auto& branch = static_cast<const BranchTerminator&>(terminator);
                std::ostringstream line;
                line << "branch " << formatValue(branch.condition())
                    << ", " << blockLabel(function, branch.trueBlockId())
                    << ", " << blockLabel(function, branch.falseBlockId());
                m_builder.appendIndentedLine(line.str());
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
                std::ostringstream line;
                line << "return " << formatValue(ret.value());
                m_builder.appendIndentedLine(line.str());
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
