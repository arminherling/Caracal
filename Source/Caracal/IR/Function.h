#pragma once

#include <Caracal/IR/BasicBlock.h>

#include <string>
#include <utility>
#include <vector>

namespace Caracal
{
    class Function
    {
    public:
        Function() = default;
        Function(std::string name, Type returnType);

        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        void setName(std::string name) { m_name = std::move(name); }

        [[nodiscard]] const std::vector<Type>& parameterTypes() const noexcept { return m_parameterTypes; }
        void addParameterType(Type type);

        [[nodiscard]] Type returnType() const noexcept { return m_returnType; }
        void setReturnType(Type returnType) noexcept { m_returnType = returnType; }

        [[nodiscard]] const std::vector<BasicBlock>& blocks() const noexcept { return m_blocks; }
        [[nodiscard]] bool hasBlocks() const noexcept { return !m_blocks.empty(); }
        [[nodiscard]] BasicBlock& firstBlock() noexcept { return m_blocks.front(); }
        [[nodiscard]] const BasicBlock& firstBlock() const noexcept { return m_blocks.front(); }
        void addBlock(BasicBlock block);

    private:
        std::string m_name;
        std::vector<Type> m_parameterTypes;
        Type m_returnType{ Type::Void() };
        std::vector<BasicBlock> m_blocks;
    };
}
