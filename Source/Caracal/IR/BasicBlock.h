#pragma once

#include <Caracal/Defines.h>
#include <Caracal/IR/Instruction.h>
#include <Caracal/IR/Terminator.h>

#include <memory>
#include <string>
#include <vector>

namespace Caracal
{
    class BasicBlock
    {
    public:
        BasicBlock() = default;
        BasicBlock(BlockId id, std::string label, TerminatorUPtr terminator);
        CARACAL_DELETE_COPY_DEFAULT_MOVE(BasicBlock)

        [[nodiscard]] BlockId id() const noexcept { return m_id; }
        [[nodiscard]] const std::string& label() const noexcept { return m_label; }
        [[nodiscard]] const std::vector<InstructionUPtr>& instructions() const noexcept { return m_instructions; }
        [[nodiscard]] const Terminator* terminator() const noexcept { return m_terminator.get(); }
        [[nodiscard]] Terminator* terminator() noexcept { return m_terminator.get(); }
        [[nodiscard]] bool hasTerminator() const noexcept { return m_terminator != nullptr; }

        void addInstruction(InstructionUPtr instruction);
        template <typename TPredicate>
        bool removeInstructions(TPredicate&& shouldRemove)
        {
            const auto sizeBefore = m_instructions.size();
            std::erase_if(m_instructions, [&shouldRemove](const InstructionUPtr& instruction)
            {
                return shouldRemove(*instruction);
            });

            return m_instructions.size() != sizeBefore;
        }
        void addPrologueInstruction(InstructionUPtr instruction);
        void setTerminator(TerminatorUPtr terminator);

    private:
        BlockId m_id{ 0 };
        std::string m_label;
        std::vector<InstructionUPtr> m_instructions;
        TerminatorUPtr m_terminator;
    };

    using BasicBlockUPtr = std::unique_ptr<BasicBlock>;
}
