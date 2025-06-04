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
        static const std::unordered_map<BinaryOperatorKind, QString> opToString{
            { BinaryOperatorKind::Addition,        QStringLiteral("Addition") },
            { BinaryOperatorKind::Subtraction,     QStringLiteral("Subtraction") },
            { BinaryOperatorKind::Multiplication,  QStringLiteral("Multiplication") },
            { BinaryOperatorKind::Division,        QStringLiteral("Division") }
        };

        const auto it = opToString.find(operation);
        if (it != opToString.end())
            return it->second;

        TODO("Invalid binary operator kind");
        return QString();
    }
}