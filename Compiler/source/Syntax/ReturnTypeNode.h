#pragma once

#include <Compiler/API.h>
#include <Syntax/Node.h>
#include <Syntax/Token.h>

namespace Caracal
{
    class COMPILER_API ReturnTypeNode : public Node
    {
    public:
        ReturnTypeNode(const Token& typeName, const Type& type);
      
        [[nodiscard]] const Token& typeName() const noexcept { return m_typeName; }

    private:
        Token m_typeName;
    };

    using ReturnTypeNodeUPtr = std::unique_ptr<ReturnTypeNode>;
}
