#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Node.h>
#include <Caracal/Syntax/NameExpression.h>
#include <Caracal/Syntax/TokenBuffer.h>
#include <Caracal/Syntax/TypeNameNode.h>
#include <Caracal/Syntax/Token.h>
#include <memory>

namespace Caracal
{
    class CARACAL_API ParameterNode : public Node
    {
    public:
        ParameterNode(
            const Token& nameToken,
            std::string_view name,
            const Token& colonToken,
            TypeNameNodeUPtr&& typeName,
            bool isVariadic = false);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(ParameterNode)

        [[nodiscard]] const Token& nameToken() const noexcept { return m_nameToken; }
        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        [[nodiscard]] const Token& colonToken() const noexcept { return m_colonToken; }
        [[nodiscard]] const TypeNameNodeUPtr& typeName() const noexcept { return m_typeName; }
        [[nodiscard]] bool isVariadic() const noexcept { return m_isVariadic; }
        [[nodiscard]] SourceLocation sourceLocation(const TokenBuffer& tokens) const;

    private:
        Token m_nameToken;
        std::string m_name;
        Token m_colonToken;
        TypeNameNodeUPtr m_typeName;
        bool m_isVariadic = false;
    };

    using ParameterNodeUPtr = std::unique_ptr<ParameterNode>;
}
