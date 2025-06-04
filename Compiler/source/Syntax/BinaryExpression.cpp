#include "BinaryExpression.h"

namespace Caracal
{
    static BinaryOperatorKind ConvertToBinaryOperator(TokenKind kind)
    {
        switch (kind)
        {
            case TokenKind::Plus:
                return BinaryOperatorKind::Addition;
            case TokenKind::Minus:
                return BinaryOperatorKind::Subtraction;
            case TokenKind::Star:
                return BinaryOperatorKind::Multiplication;
            case TokenKind::Slash:
                return BinaryOperatorKind::Division;
            default:
                return BinaryOperatorKind::Invalid;
        }
    }

    BinaryExpression::BinaryExpression(
        ExpressionUPtr&& leftExpression,
        const Token& binaryOperatorToken,
        ExpressionUPtr&& rightExpression)
        : Expression(NodeKind::BinaryExpression, Type::Undefined())
        , m_leftExpression{ std::move(leftExpression) }
        , m_binaryOperatorToken{ binaryOperatorToken }
        , m_binaryOperator{ ConvertToBinaryOperator(binaryOperatorToken.kind)}
        , m_rightExpression{ std::move(rightExpression) }
    {
    }

    QString stringify(BinaryOperatorKind operation)
    {
        switch (operation)
        {
            case BinaryOperatorKind::Addition:
                return QString("Addition");
            case BinaryOperatorKind::Subtraction:
                return QString("Subtraction");
            case BinaryOperatorKind::Multiplication:
                return QString("Multiplication");
            case BinaryOperatorKind::Division:
                return QString("Division");
            default:
                TODO("Invalid binary operator kind");
        }
        return QString();
    }
}