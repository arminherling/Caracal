#include <Caracal/Optimization/DeadCodeElimination.h>

#include <Caracal/IR/AddInstruction.h>
#include <Caracal/IR/AddressOfFieldInstruction.h>
#include <Caracal/IR/AddressOfGlobalInstruction.h>
#include <Caracal/IR/AddressOfInstruction.h>
#include <Caracal/IR/BasicBlock.h>
#include <Caracal/IR/BranchIfTerminator.h>
#include <Caracal/IR/CallInstruction.h>
#include <Caracal/IR/CallVoidInstruction.h>
#include <Caracal/IR/ConstantInstruction.h>
#include <Caracal/IR/DivideInstruction.h>
#include <Caracal/IR/EqualInstruction.h>
#include <Caracal/IR/Function.h>
#include <Caracal/IR/GreaterOrEqualInstruction.h>
#include <Caracal/IR/GreaterThanInstruction.h>
#include <Caracal/IR/IntToFloatInstruction.h>
#include <Caracal/IR/LessOrEqualInstruction.h>
#include <Caracal/IR/LessThanInstruction.h>
#include <Caracal/IR/LoadValueInstruction.h>
#include <Caracal/IR/LogicalAndInstruction.h>
#include <Caracal/IR/LogicalNegationInstruction.h>
#include <Caracal/IR/LogicalOrInstruction.h>
#include <Caracal/IR/MultiplyInstruction.h>
#include <Caracal/IR/NotEqualInstruction.h>
#include <Caracal/IR/ParameterInstruction.h>
#include <Caracal/IR/PhiInstruction.h>
#include <Caracal/IR/ReturnValueTerminator.h>
#include <Caracal/IR/StoreValueInstruction.h>
#include <Caracal/IR/SubtractInstruction.h>
#include <Caracal/IR/ValueNegationInstruction.h>

#include <unordered_set>

namespace Caracal
{
    namespace
    {
        void CollectInstructionUses(const Instruction& instruction, std::unordered_set<TemporaryId>& usedIds)
        {
            switch (instruction.kind())
            {
                case InstructionKind::FieldAddress:
                {
                    usedIds.insert(static_cast<const AddressOfFieldInstruction&>(instruction).objectAddress().id());
                    break;
                }
                case InstructionKind::LoadValue:
                {
                    usedIds.insert(static_cast<const LoadValueInstruction&>(instruction).address().id());
                    break;
                }
                case InstructionKind::StoreValue:
                {
                    const auto& store = static_cast<const StoreValueInstruction&>(instruction);
                    usedIds.insert(store.value().id());
                    usedIds.insert(store.address().id());
                    break;
                }
                case InstructionKind::ValueNegation:
                {
                    usedIds.insert(static_cast<const ValueNegationInstruction&>(instruction).operandValue().id());
                    break;
                }
                case InstructionKind::IntToFloat:
                {
                    usedIds.insert(static_cast<const IntToFloatInstruction&>(instruction).operandValue().id());
                    break;
                }
                case InstructionKind::LogicalNegation:
                {
                    usedIds.insert(static_cast<const LogicalNegationInstruction&>(instruction).operandValue().id());
                    break;
                }
                case InstructionKind::Call:
                {
                    for (const auto& argument : static_cast<const CallInstruction&>(instruction).arguments())
                    {
                        usedIds.insert(argument.id());
                    }
                    break;
                }
                case InstructionKind::CallVoid:
                {
                    for (const auto& argument : static_cast<const CallVoidInstruction&>(instruction).arguments())
                    {
                        usedIds.insert(argument.id());
                    }
                    break;
                }
                case InstructionKind::Add:
                {
                    const auto& add = static_cast<const AddInstruction&>(instruction);
                    usedIds.insert(add.leftValue().id());
                    usedIds.insert(add.rightValue().id());
                    break;
                }
                case InstructionKind::Subtract:
                {
                    const auto& subtract = static_cast<const SubtractInstruction&>(instruction);
                    usedIds.insert(subtract.leftValue().id());
                    usedIds.insert(subtract.rightValue().id());
                    break;
                }
                case InstructionKind::Multiply:
                {
                    const auto& multiply = static_cast<const MultiplyInstruction&>(instruction);
                    usedIds.insert(multiply.leftValue().id());
                    usedIds.insert(multiply.rightValue().id());
                    break;
                }
                case InstructionKind::Divide:
                {
                    const auto& divide = static_cast<const DivideInstruction&>(instruction);
                    usedIds.insert(divide.leftValue().id());
                    usedIds.insert(divide.rightValue().id());
                    break;
                }
                case InstructionKind::Equal:
                {
                    const auto& equal = static_cast<const EqualInstruction&>(instruction);
                    usedIds.insert(equal.leftValue().id());
                    usedIds.insert(equal.rightValue().id());
                    break;
                }
                case InstructionKind::NotEqual:
                {
                    const auto& notEqual = static_cast<const NotEqualInstruction&>(instruction);
                    usedIds.insert(notEqual.leftValue().id());
                    usedIds.insert(notEqual.rightValue().id());
                    break;
                }
                case InstructionKind::LessThan:
                {
                    const auto& lessThan = static_cast<const LessThanInstruction&>(instruction);
                    usedIds.insert(lessThan.leftValue().id());
                    usedIds.insert(lessThan.rightValue().id());
                    break;
                }
                case InstructionKind::LessOrEqual:
                {
                    const auto& lessOrEqual = static_cast<const LessOrEqualInstruction&>(instruction);
                    usedIds.insert(lessOrEqual.leftValue().id());
                    usedIds.insert(lessOrEqual.rightValue().id());
                    break;
                }
                case InstructionKind::GreaterThan:
                {
                    const auto& greaterThan = static_cast<const GreaterThanInstruction&>(instruction);
                    usedIds.insert(greaterThan.leftValue().id());
                    usedIds.insert(greaterThan.rightValue().id());
                    break;
                }
                case InstructionKind::GreaterOrEqual:
                {
                    const auto& greaterOrEqual = static_cast<const GreaterOrEqualInstruction&>(instruction);
                    usedIds.insert(greaterOrEqual.leftValue().id());
                    usedIds.insert(greaterOrEqual.rightValue().id());
                    break;
                }
                case InstructionKind::LogicalAnd:
                {
                    const auto& logicalAnd = static_cast<const LogicalAndInstruction&>(instruction);
                    usedIds.insert(logicalAnd.leftValue().id());
                    usedIds.insert(logicalAnd.rightValue().id());
                    break;
                }
                case InstructionKind::LogicalOr:
                {
                    const auto& logicalOr = static_cast<const LogicalOrInstruction&>(instruction);
                    usedIds.insert(logicalOr.leftValue().id());
                    usedIds.insert(logicalOr.rightValue().id());
                    break;
                }
                case InstructionKind::Phi:
                {
                    for (const auto& input : static_cast<const PhiInstruction&>(instruction).inputs())
                    {
                        usedIds.insert(input.value().id());
                    }
                    break;
                }
                default:
                {
                    break;
                }
            }
        }

        void CollectTerminatorUses(const Terminator* terminator, std::unordered_set<TemporaryId>& usedIds)
        {
            if (terminator == nullptr)
            {
                return;
            }

            switch (terminator->kind())
            {
                case TerminatorKind::Branch:
                {
                    usedIds.insert(static_cast<const BranchIfTerminator*>(terminator)->condition().id());
                    break;
                }
                case TerminatorKind::ReturnValue:
                {
                    usedIds.insert(static_cast<const ReturnValueTerminator*>(terminator)->value().id());
                    break;
                }
                default:
                {
                    break;
                }
            }
        }

        [[nodiscard]] bool IsRemovableWhenUnused(const Instruction& instruction)
        {
            switch (instruction.kind())
            {
                case InstructionKind::Constant:
                case InstructionKind::ValueNegation:
                case InstructionKind::IntToFloat:
                case InstructionKind::LogicalNegation:
                case InstructionKind::Add:
                case InstructionKind::Subtract:
                case InstructionKind::Multiply:
                case InstructionKind::Divide:
                case InstructionKind::Equal:
                case InstructionKind::NotEqual:
                case InstructionKind::LessThan:
                case InstructionKind::LessOrEqual:
                case InstructionKind::GreaterThan:
                case InstructionKind::GreaterOrEqual:
                case InstructionKind::LogicalAnd:
                case InstructionKind::LogicalOr:
                case InstructionKind::Phi:
                {
                    return true;
                }
                default:
                {
                    return false;
                }
            }
        }

        [[nodiscard]] bool TryGetInstructionResultId(const Instruction& instruction, TemporaryId& resultId) noexcept
        {
            switch (instruction.kind())
            {
                case InstructionKind::Parameter:
                {
                    resultId = static_cast<const ParameterInstruction&>(instruction).resultId();
                    return true;
                }
                case InstructionKind::Constant:
                {
                    resultId = static_cast<const ConstantInstruction&>(instruction).resultId();
                    return true;
                }
                case InstructionKind::AddressOf:
                {
                    resultId = static_cast<const AddressOfInstruction&>(instruction).resultId();
                    return true;
                }
                case InstructionKind::AddressOfGlobal:
                {
                    resultId = static_cast<const AddressOfGlobalInstruction&>(instruction).resultId();
                    return true;
                }
                case InstructionKind::FieldAddress:
                {
                    resultId = static_cast<const AddressOfFieldInstruction&>(instruction).resultId();
                    return true;
                }
                case InstructionKind::LoadValue:
                {
                    resultId = static_cast<const LoadValueInstruction&>(instruction).resultId();
                    return true;
                }
                case InstructionKind::ValueNegation:
                {
                    resultId = static_cast<const ValueNegationInstruction&>(instruction).resultId();
                    return true;
                }
                case InstructionKind::IntToFloat:
                {
                    resultId = static_cast<const IntToFloatInstruction&>(instruction).resultId();
                    return true;
                }
                case InstructionKind::LogicalNegation:
                {
                    resultId = static_cast<const LogicalNegationInstruction&>(instruction).resultId();
                    return true;
                }
                case InstructionKind::Call:
                {
                    resultId = static_cast<const CallInstruction&>(instruction).resultId();
                    return true;
                }
                case InstructionKind::Add:
                {
                    resultId = static_cast<const AddInstruction&>(instruction).resultId();
                    return true;
                }
                case InstructionKind::Subtract:
                {
                    resultId = static_cast<const SubtractInstruction&>(instruction).resultId();
                    return true;
                }
                case InstructionKind::Multiply:
                {
                    resultId = static_cast<const MultiplyInstruction&>(instruction).resultId();
                    return true;
                }
                case InstructionKind::Divide:
                {
                    resultId = static_cast<const DivideInstruction&>(instruction).resultId();
                    return true;
                }
                case InstructionKind::Equal:
                {
                    resultId = static_cast<const EqualInstruction&>(instruction).resultId();
                    return true;
                }
                case InstructionKind::NotEqual:
                {
                    resultId = static_cast<const NotEqualInstruction&>(instruction).resultId();
                    return true;
                }
                case InstructionKind::LessThan:
                {
                    resultId = static_cast<const LessThanInstruction&>(instruction).resultId();
                    return true;
                }
                case InstructionKind::LessOrEqual:
                {
                    resultId = static_cast<const LessOrEqualInstruction&>(instruction).resultId();
                    return true;
                }
                case InstructionKind::GreaterThan:
                {
                    resultId = static_cast<const GreaterThanInstruction&>(instruction).resultId();
                    return true;
                }
                case InstructionKind::GreaterOrEqual:
                {
                    resultId = static_cast<const GreaterOrEqualInstruction&>(instruction).resultId();
                    return true;
                }
                case InstructionKind::LogicalAnd:
                {
                    resultId = static_cast<const LogicalAndInstruction&>(instruction).resultId();
                    return true;
                }
                case InstructionKind::LogicalOr:
                {
                    resultId = static_cast<const LogicalOrInstruction&>(instruction).resultId();
                    return true;
                }
                case InstructionKind::Phi:
                {
                    resultId = static_cast<const PhiInstruction&>(instruction).resultId();
                    return true;
                }
                default:
                {
                    return false;
                }
            }
        }

        [[nodiscard]] bool IsDeadInstruction(const Instruction& instruction, const std::unordered_set<TemporaryId>& usedIds)
        {
            if (!IsRemovableWhenUnused(instruction))
            {
                return false;
            }

            TemporaryId resultId{};
            if (!TryGetInstructionResultId(instruction, resultId))
            {
                return false;
            }

            return !usedIds.contains(resultId);
        }

        [[nodiscard]] bool EliminateInFunction(Function& function)
        {
            std::unordered_set<TemporaryId> usedIds{};
            for (const auto& block : function.blocks())
            {
                for (const auto& instruction : block->instructions())
                {
                    CollectInstructionUses(*instruction, usedIds);
                }

                CollectTerminatorUses(block->terminator(), usedIds);
            }

            auto removedAny = false;
            for (const auto& block : function.blocks())
            {
                const auto removedInBlock = block->removeInstructions(
                    [&usedIds](const Instruction& instruction)
                    {
                        return IsDeadInstruction(instruction, usedIds);
                    });

                if (removedInBlock)
                {
                    removedAny = true;
                }
            }

            return removedAny;
        }

        void RenumberFunction(Function& function)
        {
            ValueIdMap remap{};
            auto nextId = TemporaryId{ 0 };
            for (const auto& block : function.blocks())
            {
                for (const auto& instruction : block->instructions())
                {
                    TemporaryId resultId{};
                    if (TryGetInstructionResultId(*instruction, resultId))
                    {
                        remap.emplace(resultId, nextId);
                        ++nextId;
                    }
                }
            }

            for (const auto& block : function.blocks())
            {
                for (const auto& instruction : block->instructions())
                {
                    instruction->remapValueIds(remap);
                }

                auto* terminator = block->terminator();
                if (terminator != nullptr)
                {
                    terminator->remapValueIds(remap);
                }
            }
        }
    }

    void eliminateDeadCode(Module& module) noexcept
    {
        for (auto& function : module.functions())
        {
            // removing an instruction can strand its operands, so run to a fixed point
            while (EliminateInFunction(function))
            {
            }

            // elimination leaves gaps in the value ids, renumber them densely in definition order
            RenumberFunction(function);
        }
    }
}
