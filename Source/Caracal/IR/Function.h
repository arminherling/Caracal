#pragma once

#include <Caracal/Defines.h>
#include <Caracal/IR/BasicBlock.h>

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Caracal
{
    class Function
    {
    public:
        Function() = default;
        Function(std::string name, const std::vector<Type>& parameterTypes, Type returnType);
        CARACAL_DELETE_COPY_DEFAULT_MOVE(Function)

        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        [[nodiscard]] const std::vector<Type>& parameterTypes() const noexcept { return m_parameterTypes; }
        [[nodiscard]] Type returnType() const noexcept { return m_returnType; }
        [[nodiscard]] const std::vector<BasicBlockUPtr>& blocks() const noexcept { return m_blocks; }
        [[nodiscard]] bool hasBlocks() const noexcept { return !m_blocks.empty(); }
        [[nodiscard]] const BasicBlock& firstBlock() const noexcept { return *m_blocks.front(); }
        [[nodiscard]] BasicBlock* tryGetBlock(BlockId id) noexcept;
        [[nodiscard]] const BasicBlock* tryGetBlock(BlockId id) const noexcept;
        
        void addParameterType(Type type);
        void addBlock(BasicBlock block);

    private:
        std::string m_name;
        std::vector<Type> m_parameterTypes;
        Type m_returnType{ Type::Void() };
        std::vector<BasicBlockUPtr> m_blocks;
        std::unordered_map<BlockId, size_t> m_blockIndices;
    };
}
