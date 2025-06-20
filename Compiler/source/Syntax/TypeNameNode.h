#pragma once

#include <Compiler/API.h>
#include <Syntax/Node.h>
#include <Syntax/NameExpression.h>
#include <optional>

namespace Caracal 
{
    class COMPILER_API TypeNameNode : public Node
    {
    public:
        TypeNameNode(
            const std::optional<Token>& refToken, 
            NameExpressionUPtr&& name,
            const Type& type);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(TypeNameNode)

        [[nodiscard]] const std::optional<Token>& ref() const noexcept { return m_refToken; }
        [[nodiscard]] bool isReference() const noexcept { return m_refToken.has_value(); }
        [[nodiscard]] const NameExpressionUPtr& name() const noexcept { return m_nameExpression; }

    private:
        std::optional<Token> m_refToken;
        NameExpressionUPtr m_nameExpression;
    };

    using TypeNameNodeUPtr = std::unique_ptr<TypeNameNode>;
}
