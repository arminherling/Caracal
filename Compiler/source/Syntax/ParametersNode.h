#pragma once

#include <Compiler/API.h>
#include <Syntax/ParameterNode.h>
#include <Syntax/Token.h>
#include <vector>

namespace Caracal
{
    class COMPILER_API ParametersNode : public Node
    {
    public:
        ParametersNode(
            const Token& openParenthesisToken,
            std::vector<ParameterNodeUPtr>&& parameters,
            const Token& closeParenthesisToken);

        ParametersNode(const ParametersNode&) = delete;
        ParametersNode& operator=(const ParametersNode&) = delete;

        ParametersNode(ParametersNode&&) = default;
        ParametersNode& operator=(ParametersNode&&) = default;

        [[nodiscard]] const Token& openParenthesisToken() const noexcept { return m_openParenthesisToken; }
        [[nodiscard]] const std::vector<ParameterNodeUPtr>& parameters() const noexcept { return m_parameters; }
        [[nodiscard]] const Token& closeParenthesisToken() const noexcept { return m_closeParenthesisToken; }

    private:
        Token m_openParenthesisToken;
        std::vector<ParameterNodeUPtr> m_parameters;
        Token m_closeParenthesisToken;
    };

    using ParametersNodeUPtr = std::unique_ptr<ParametersNode>;
}
