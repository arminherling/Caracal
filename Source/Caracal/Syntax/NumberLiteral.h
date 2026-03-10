#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/TypeNameNode.h>
#include <Caracal/Syntax/Token.h>
#include <string>
#include <string_view>

namespace Caracal
{
    class CARACAL_API NumberLiteral : public Expression
    {
    public:
        NumberLiteral(
            const Token& literalToken,
            std::string_view lexeme,
            const std::optional<Token>& uptickToken,
            std::optional<TypeNameNodeUPtr>&& explicitType);

        [[nodiscard]] const Token& literalToken() const noexcept { return m_literalToken; }
        [[nodiscard]] const std::optional<Token>& uptickToken() const noexcept { return m_uptickToken; }
        [[nodiscard]] const std::optional<TypeNameNodeUPtr>& explicitType() const noexcept { return m_explicitType; }
        [[nodiscard]] const std::string& literalLexeme() const noexcept { return m_lexeme; }

    private:
        Token m_literalToken;
        std::optional<Token> m_uptickToken;
        std::optional<TypeNameNodeUPtr> m_explicitType;
        std::string m_lexeme;
    };

    using NumberLiteralUPtr = std::unique_ptr<NumberLiteral>;
}
