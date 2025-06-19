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
            case TokenKind::EqualEqual:
                return BinaryOperatorKind::Equal;
            case TokenKind::BangEqual:
                return BinaryOperatorKind::NotEqual;
            case TokenKind::LessThan:
                return BinaryOperatorKind::LessThan;
            case TokenKind::LessThanEqual:
                return BinaryOperatorKind::LessOrEqual;
            case TokenKind::GreaterThan:
                return BinaryOperatorKind::GreaterThan;
            case TokenKind::GreaterThanEqual:
                return BinaryOperatorKind::GreaterOrEqual;
            case TokenKind::AndKeyword:
                return BinaryOperatorKind::LogicalAnd;
            case TokenKind::OrKeyword:
                return BinaryOperatorKind::LogicalOr;
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
            { BinaryOperatorKind::Division,        QStringLiteral("Division") },
            { BinaryOperatorKind::Equal,           QStringLiteral("Equal") },
            { BinaryOperatorKind::NotEqual,        QStringLiteral("NotEqual") },
            { BinaryOperatorKind::LessThan,        QStringLiteral("LessThan") },
            { BinaryOperatorKind::LessOrEqual,     QStringLiteral("LessOrEqual") },
            { BinaryOperatorKind::GreaterThan,     QStringLiteral("GreaterThan") },
            { BinaryOperatorKind::GreaterOrEqual,  QStringLiteral("GreaterOrEqual") },
            { BinaryOperatorKind::LogicalAnd,      QStringLiteral("LogicalAnd") },
            { BinaryOperatorKind::LogicalOr,       QStringLiteral("LogicalOr") },
        };

        const auto it = opToString.find(operation);
        if (it != opToString.end())
            return it->second;

        TODO("Invalid binary operator kind");
        return QString();
    }
}