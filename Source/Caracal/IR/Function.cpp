#include <Caracal/IR/Function.h>

namespace Caracal
{
    Function::Function(std::string name, const std::vector<Type>& parameterTypes, Type returnType)
        : m_name{ std::move(name) }
        , m_parameterTypes{ parameterTypes }
        , m_returnType{ returnType }
    {
    }

    BasicBlock* Function::tryGetBlock(BlockId id) noexcept
    {
        const auto result = m_blockIndices.find(id);
        if (result == m_blockIndices.end())
            return nullptr;

        return m_blocks[result->second].get();
    }

    const BasicBlock* Function::tryGetBlock(BlockId id) const noexcept
    {
        const auto result = m_blockIndices.find(id);
        if (result == m_blockIndices.end())
            return nullptr;

        return m_blocks[result->second].get();
    }

    void Function::addParameterType(Type type)
    {
        m_parameterTypes.push_back(type);
    }

    void Function::addBlock(BasicBlock block)
    {
        const auto blockId = block.id();
        m_blockIndices.emplace(blockId, m_blocks.size());
        m_blocks.push_back(std::make_unique<BasicBlock>(std::move(block)));
    }
}
