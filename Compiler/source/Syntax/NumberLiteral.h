#pragma once

#include <Compiler/API.h>
#include <Syntax/Expression.h>
#include <Syntax/TypeNameNode.h>
#include <Syntax/Token.h>

namespace Caracal
{
    class COMPILER_API NumberLiteral : public Expression
    {
    public:
        NumberLiteral(
            const Token& literalToken,
            const std::optional<Token>& uptickToken,
            std::optional<TypeNameNodeUPtr>&& explicitType);

        [[nodiscard]] const Token& literalToken() const noexcept { return m_literalToken; }
        [[nodiscard]] const std::optional<Token>& uptickToken() const noexcept { return m_uptickToken; }
        [[nodiscard]] const std::optional<TypeNameNodeUPtr>& explicitType() const noexcept { return m_explicitType; }

    private:
        Token m_literalToken;
        std::optional<Token> m_uptickToken;
        std::optional<TypeNameNodeUPtr> m_explicitType;
    };
}
