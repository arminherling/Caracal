#pragma once

#include <Caracal/IR/Instruction.h>
#include <Caracal/IR/Terminator.h>

#include <string>
#include <vector>

namespace Caracal
{
    class BasicBlock
    {
    public:
        BasicBlock() = default;
        BasicBlock(BlockId id, std::string label, TerminatorUPtr terminator);

        [[nodiscard]] BlockId id() const noexcept { return m_id; }
        [[nodiscard]] const std::string& label() const noexcept { return m_label; }
        [[nodiscard]] const std::vector<InstructionUPtr>& instructions() const noexcept { return m_instructions; }
        [[nodiscard]] const Terminator* terminator() const noexcept { return m_terminator.get(); }
        [[nodiscard]] bool hasTerminator() const noexcept { return m_terminator != nullptr; }

        void addInstruction(InstructionUPtr instruction);
        void setTerminator(TerminatorUPtr terminator);

    private:
        BlockId m_id{ 0 };
        std::string m_label;
        std::vector<InstructionUPtr> m_instructions;
        TerminatorUPtr m_terminator;
    };
}
