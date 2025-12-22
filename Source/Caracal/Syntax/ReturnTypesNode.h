#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/TypeNameNode.h>
#include <vector>

namespace Caracal
{
    class CARACAL_API ReturnTypesNode : public Node
    {
    public:
        explicit ReturnTypesNode(std::vector<TypeNameNodeUPtr>&& returnTypes);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(ReturnTypesNode)

        [[nodiscard]] const std::vector<TypeNameNodeUPtr>& returnTypes() const noexcept { return m_returnTypes; }

    private:
        std::vector<TypeNameNodeUPtr> m_returnTypes;
    };

    using ReturnTypesNodeUPtr = std::unique_ptr<ReturnTypesNode>;
}
