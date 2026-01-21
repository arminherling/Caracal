#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/NameExpression.h>
#include <Caracal/Syntax/Token.h>
#include <Caracal/Syntax/Node.h>

namespace Caracal
{
    class CARACAL_API MethodNameNode : public Node
    {
    public:
        MethodNameNode(
            const Token& methodNameToken,
            std::string_view methodName);
        MethodNameNode(
            const Token& typeNameToken,
            std::string_view typeName,
            const Token& dotToken,
            const Token& methodNameToken,
            std::string_view methodName);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(MethodNameNode)

        [[nodiscard]] const std::optional<Token>& typeNameToken() const noexcept { return m_typeNameToken; }
        [[nodiscard]] const std::optional<std::string>& typeName() const noexcept { return m_typeName; }
        [[nodiscard]] const std::optional<Token>& dotToken() const noexcept { return m_dotToken; }
        [[nodiscard]] const Token& methodNameToken() const noexcept { return m_methodNameToken; }
        [[nodiscard]] const std::string& methodName() const noexcept { return m_methodName; }
        [[nodiscard]] bool hasTypeName() const noexcept { return m_typeName.has_value(); }

    private:
        std::optional<Token> m_typeNameToken;
        std::optional<std::string> m_typeName;
        std::optional<Token> m_dotToken;
        Token m_methodNameToken;
        std::string m_methodName;
    };

    using MethodNameNodeUPtr = std::unique_ptr<MethodNameNode>;
}
