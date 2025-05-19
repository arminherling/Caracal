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
            const Token& returnKeyword,
            std::optional<ExpressionUPtr>&& expression,
            const Token& semicolon);

        ReturnStatement(const ReturnStatement&) = delete;
        ReturnStatement& operator=(const ReturnStatement&) = delete;
        
        ReturnStatement(ReturnStatement&&) = default;
        ReturnStatement& operator=(ReturnStatement&&) = default;

        [[nodiscard]] const Token& returnKeyword() const noexcept { return m_returnKeyword; }
        [[nodiscard]] const std::optional<ExpressionUPtr>& expression() const noexcept { return m_expression; }
        [[nodiscard]] const Token& semicolon() const noexcept { return m_semicolon; }

    private:
        Token m_returnKeyword;
        std::optional<ExpressionUPtr> m_expression;
        Token m_semicolon;
    };
}
