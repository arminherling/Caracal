#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/Token.h>

#include <optional>
#include <vector>

namespace Caracal
{
    class CARACAL_API ArrayLiteral final : public Expression
    {
    public:
        ArrayLiteral(
            const Token& openBracketToken,
            std::vector<ExpressionUPtr> elements,
            const std::optional<Token>& ellipsisToken,
            const Token& closeBracketToken);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(ArrayLiteral)

        [[nodiscard]] const Token& openBracketToken() const noexcept { return m_openBracketToken; }
        [[nodiscard]] const std::vector<ExpressionUPtr>& elements() const noexcept { return m_elements; }
        [[nodiscard]] const std::optional<Token>& ellipsisToken() const noexcept { return m_ellipsisToken; }
        [[nodiscard]] bool isDynamic() const noexcept { return m_ellipsisToken.has_value(); }
        [[nodiscard]] const Token& closeBracketToken() const noexcept { return m_closeBracketToken; }
        [[nodiscard]] SourceLocation sourceLocation(const TokenBuffer& tokens) const override;

    private:
        Token m_openBracketToken;
        std::vector<ExpressionUPtr> m_elements;
        std::optional<Token> m_ellipsisToken;
        Token m_closeBracketToken;
    };

    using ArrayLiteralUPtr = std::unique_ptr<ArrayLiteral>;
}
