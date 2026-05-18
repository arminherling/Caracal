#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/TypeNameNode.h>
#include <Caracal/Syntax/Token.h>

#include <optional>
#include <string>
#include <string_view>
#include <variant>

namespace Caracal
{
    class CARACAL_API NumberLiteral : public Expression
    {
    public:
        using ParsedValue = std::variant<u8, i32, f32>;

        NumberLiteral(
            const Token& literalToken,
            std::string_view lexeme,
            const std::optional<Token>& uptickToken,
            std::optional<TypeNameNodeUPtr>&& explicitType);

        [[nodiscard]] const Token& literalToken() const noexcept { return m_literalToken; }
        [[nodiscard]] const std::optional<Token>& uptickToken() const noexcept { return m_uptickToken; }
        [[nodiscard]] const std::optional<TypeNameNodeUPtr>& explicitType() const noexcept { return m_explicitType; }
        [[nodiscard]] const std::string& literalLexeme() const noexcept { return m_lexeme; }
        [[nodiscard]] SourceLocation sourceLocation(const TokenBuffer& tokens) const override;
        [[nodiscard]] bool hasParsedValue() const noexcept { return m_parsedValue.has_value(); }
        [[nodiscard]] const std::optional<ParsedValue>& parsedValue() const noexcept { return m_parsedValue; }
        void setParsedValue(std::optional<ParsedValue> value) noexcept;

    private:
        Token m_literalToken;
        std::optional<Token> m_uptickToken;
        std::optional<TypeNameNodeUPtr> m_explicitType;
        std::string m_lexeme;
        std::optional<ParsedValue> m_parsedValue;
    };

    using NumberLiteralUPtr = std::unique_ptr<NumberLiteral>;
}
