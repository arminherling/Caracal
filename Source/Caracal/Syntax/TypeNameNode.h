#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Node.h>
#include <Caracal/Syntax/NameExpression.h>
#include <optional>

namespace Caracal 
{
    class CARACAL_API TypeNameNode : public Node
    {
    public:
        TypeNameNode(
            const std::optional<Token>& refToken, 
            const Token& nameToken,
            std::string_view name);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(TypeNameNode)

        [[nodiscard]] const std::optional<Token>& ref() const noexcept { return m_refToken; }
        [[nodiscard]] bool isReference() const noexcept { return m_refToken.has_value(); }
        [[nodiscard]] const Token& nameToken() const noexcept { return m_nameToken; }
        [[nodiscard]] const std::string& name() const noexcept { return m_name; }

    private:
        std::optional<Token> m_refToken;
        Token m_nameToken;
        std::string m_name;
    };

    using TypeNameNodeUPtr = std::unique_ptr<TypeNameNode>;
}
