#pragma once

#include <Compiler/API.h>
#include <Syntax/Statement.h>
#include <Syntax/Token.h>
#include <Syntax/Expression.h>

namespace Caracal
{
    class COMPILER_API ReturnStatement : public Statement
    {
    public:
        ReturnStatement(
            const Token& keywordToken,
            std::optional<ExpressionUPtr>&& expression,
            const Token& semicolonToken);

        ReturnStatement(const ReturnStatement&) = delete;
        ReturnStatement& operator=(const ReturnStatement&) = delete;
        
        ReturnStatement(ReturnStatement&&) = default;
        ReturnStatement& operator=(ReturnStatement&&) = default;

        [[nodiscard]] const Token& keywordToken() const noexcept { return m_keywordToken; }
        [[nodiscard]] const std::optional<ExpressionUPtr>& expression() const noexcept { return m_expression; }
        [[nodiscard]] const Token& semicolonToken() const noexcept { return m_semicolonToken; }

    private:
        Token m_keywordToken;
        std::optional<ExpressionUPtr> m_expression;
        Token m_semicolonToken;
    };
}
