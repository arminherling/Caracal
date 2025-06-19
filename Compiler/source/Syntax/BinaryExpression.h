#pragma once

#include <Compiler/API.h>
#include <Syntax/Expression.h>
#include <Syntax/Token.h>

namespace Caracal
{
    enum class BinaryOperatorKind
    {
        Invalid,
        MemberAccess,
        Addition,
        Subtraction,
        Multiplication,
        Division,
        Equal,
        NotEqual,
        LessThan,
        LessOrEqual,
        GreaterThan,
        GreaterOrEqual,
        LogicalAnd,
        LogicalOr,
    };

    class COMPILER_API BinaryExpression : public Expression
    {
    public:
        BinaryExpression(
            ExpressionUPtr&& leftExpression,
            const Token& binaryOperatorToken,
            ExpressionUPtr&& rightExpression);

        [[nodiscard]] const ExpressionUPtr& leftExpression() const noexcept { return m_leftExpression; }
        [[nodiscard]] const Token& binaryOperatorToken() const noexcept { return m_binaryOperatorToken; }
        [[nodiscard]] const ExpressionUPtr& rightExpression() const noexcept { return m_rightExpression; }
        [[nodiscard]] BinaryOperatorKind binaryOperator() const noexcept { return m_binaryOperator; }

    private:
        ExpressionUPtr m_leftExpression;
        Token m_binaryOperatorToken;
        ExpressionUPtr m_rightExpression;
        BinaryOperatorKind m_binaryOperator;
    };

    COMPILER_API [[nodiscard]] QString stringify(BinaryOperatorKind kind);
}
