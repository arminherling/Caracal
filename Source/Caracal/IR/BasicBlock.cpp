#include <Caracal/IR/BasicBlock.h>

namespace Caracal
{
    BasicBlock::BasicBlock(BlockId id, std::string label, TerminatorUPtr terminator)
        : m_id{ id }
        , m_label{ std::move(label) }
        , m_terminator{ std::move(terminator) }
    {
    }

    void BasicBlock::addInstruction(InstructionUPtr instruction)
    {
        m_instructions.push_back(std::move(instruction));
    }

    void BasicBlock::addPrologueInstruction(InstructionUPtr instruction)
    {
        auto insertPosition = m_instructions.begin();
        while (insertPosition != m_instructions.end())
        {
            const auto kind = (*insertPosition)->kind();
            if (kind != InstructionKind::Parameter && kind != InstructionKind::AllocateLocal)
                break;

            ++insertPosition;
        }

        m_instructions.insert(insertPosition, std::move(instruction));
    }

    void BasicBlock::setTerminator(TerminatorUPtr terminator)
    {
        m_terminator = std::move(terminator);
    }
}
