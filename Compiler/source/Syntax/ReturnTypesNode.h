#pragma once

#include <Compiler/API.h>
#include <Syntax/TypeNameNode.h>
#include <vector>

namespace Caracal
{
    class COMPILER_API ReturnTypesNode : public Node
    {
    public:
        explicit ReturnTypesNode(std::vector<TypeNameNodeUPtr>&& returnTypes);

        ReturnTypesNode(const ReturnTypesNode&) = delete;
        ReturnTypesNode& operator=(const ReturnTypesNode&) = delete;
        
        ReturnTypesNode(ReturnTypesNode&&) = default;
        ReturnTypesNode& operator=(ReturnTypesNode&&) = default;

        [[nodiscard]] const std::vector<TypeNameNodeUPtr>& returnTypes() const noexcept { return m_returnTypes; }

    private:
        std::vector<TypeNameNodeUPtr> m_returnTypes;
    };

    using ReturnTypesNodeUPtr = std::unique_ptr<ReturnTypesNode>;
}
