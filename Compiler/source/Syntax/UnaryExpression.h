#pragma once

#include <Compiler/API.h>
#include <Syntax/Expression.h>
#include <Syntax/Token.h>

namespace Caracal
{
    enum class UnaryOperatorKind
    {
        Invalid,
        LogicalNegation,
        ValueNegation,
        ReferenceOf
    };

    class COMPILER_API UnaryExpression : public Expression
    {
    public:
        UnaryExpression(
            const Token& unaryOperatorToken,
            ExpressionUPtr&& expression);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(UnaryExpression)

        [[nodiscard]] const Token& unaryOperatorToken() const noexcept { return m_unaryOperatorToken; }
        [[nodiscard]] const ExpressionUPtr& expression() const noexcept { return m_expression; }
        [[nodiscard]] UnaryOperatorKind unaryOperator() const noexcept { return m_unaryOperator; }

    private:
        Token m_unaryOperatorToken;
        ExpressionUPtr m_expression;
        UnaryOperatorKind m_unaryOperator;
    };

    [[nodiscard]] COMPILER_API std::string stringify(UnaryOperatorKind kind);
}
