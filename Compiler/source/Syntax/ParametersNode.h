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
            const Token& openParenthesis,
            std::vector<ParameterNodeUPtr>&& parameters,
            const Token& closeParenthesis);

        ParametersNode(const ParametersNode&) = delete;
        ParametersNode& operator=(const ParametersNode&) = delete;

        ParametersNode(ParametersNode&&) = default;
        ParametersNode& operator=(ParametersNode&&) = default;

        [[nodiscard]] const Token& openParenthesis() const noexcept { return m_openParenthesis; }
        [[nodiscard]] const std::vector<ParameterNodeUPtr>& parameters() const noexcept { return m_parameters; }
        [[nodiscard]] const Token& closeParenthesis() const noexcept { return m_closeParenthesis; }

    private:
        Token m_openParenthesis;
        std::vector<ParameterNodeUPtr> m_parameters;
        Token m_closeParenthesis;
    };

    using ParametersNodeUPtr = std::unique_ptr<ParametersNode>;
}
