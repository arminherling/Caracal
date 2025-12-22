#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/Token.h>

namespace Caracal
{
    enum class UnaryOperatorKind
    {
        Invalid,
        LogicalNegation,
        ValueNegation,
        ReferenceOf
    };

    class CARACAL_API UnaryExpression : public Expression
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

    [[nodiscard]] CARACAL_API std::string stringify(UnaryOperatorKind kind);
}
