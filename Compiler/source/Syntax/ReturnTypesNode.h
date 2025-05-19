#pragma once

#include <Compiler/API.h>
#include <Syntax/ReturnTypeNode.h>
#include <vector>

namespace Caracal
{
    class COMPILER_API ReturnTypesNode : public Node
    {
    public:
        explicit ReturnTypesNode(std::vector<ReturnTypeNodeUPtr>&& returnTypes);

        ReturnTypesNode(const ReturnTypesNode&) = delete;
        ReturnTypesNode& operator=(const ReturnTypesNode&) = delete;
        
        ReturnTypesNode(ReturnTypesNode&&) = default;
        ReturnTypesNode& operator=(ReturnTypesNode&&) = default;

        [[nodiscard]] const std::vector<ReturnTypeNodeUPtr>& returnTypes() const noexcept { return m_returnTypes; }

    private:
        std::vector<ReturnTypeNodeUPtr> m_returnTypes;
    };

    using ReturnTypesNodeUPtr = std::unique_ptr<ReturnTypesNode>;
}
