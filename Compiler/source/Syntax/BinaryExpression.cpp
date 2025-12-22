#include "BinaryExpression.h"

namespace Caracal
{
    static BinaryOperatorKind ConvertToBinaryOperator(TokenKind kind)
    {
        switch (kind)
        {
            case TokenKind::Dot:
                return BinaryOperatorKind::MemberAccess;
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

    std::string stringify(BinaryOperatorKind operation)
    {
        static const std::unordered_map<BinaryOperatorKind, std::string> opToString{
            { BinaryOperatorKind::MemberAccess,    std::string("MemberAccess") },
            { BinaryOperatorKind::Addition,        std::string("Addition") },
            { BinaryOperatorKind::Subtraction,     std::string("Subtraction") },
            { BinaryOperatorKind::Multiplication,  std::string("Multiplication") },
            { BinaryOperatorKind::Division,        std::string("Division") },
            { BinaryOperatorKind::Equal,           std::string("Equal") },
            { BinaryOperatorKind::NotEqual,        std::string("NotEqual") },
            { BinaryOperatorKind::LessThan,        std::string("LessThan") },
            { BinaryOperatorKind::LessOrEqual,     std::string("LessOrEqual") },
            { BinaryOperatorKind::GreaterThan,     std::string("GreaterThan") },
            { BinaryOperatorKind::GreaterOrEqual,  std::string("GreaterOrEqual") },
            { BinaryOperatorKind::LogicalAnd,      std::string("LogicalAnd") },
            { BinaryOperatorKind::LogicalOr,       std::string("LogicalOr") },
        };

        const auto it = opToString.find(operation);
        if (it != opToString.end())
            return it->second;

        TODO("Invalid binary operator kind");
        return std::string();
    }
}