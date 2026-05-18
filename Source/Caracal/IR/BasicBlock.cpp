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

    void BasicBlock::setTerminator(TerminatorUPtr terminator)
    {
        m_terminator = std::move(terminator);
    }
}
