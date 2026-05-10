#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/ParameterNode.h>
#include <Caracal/Syntax/Token.h>
#include <Caracal/Syntax/TokenBuffer.h>
#include <vector>

namespace Caracal
{
    class CARACAL_API ParametersNode : public Node
    {
    public:
        ParametersNode(
            const Token& openParenthesisToken,
            std::vector<ParameterNodeUPtr>&& parameters,
            const Token& closeParenthesisToken);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(ParametersNode)

        [[nodiscard]] const Token& openParenthesisToken() const noexcept { return m_openParenthesisToken; }
        [[nodiscard]] const std::vector<ParameterNodeUPtr>& parameters() const noexcept { return m_parameters; }
        [[nodiscard]] const Token& closeParenthesisToken() const noexcept { return m_closeParenthesisToken; }
        [[nodiscard]] SourceLocation sourceLocation(const TokenBuffer& tokens) const;

    private:
        Token m_openParenthesisToken;
        std::vector<ParameterNodeUPtr> m_parameters;
        Token m_closeParenthesisToken;
    };

    using ParametersNodeUPtr = std::unique_ptr<ParametersNode>;
}
