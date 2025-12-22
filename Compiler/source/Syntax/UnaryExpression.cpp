#include "UnaryExpression.h"

namespace Caracal
{
    static UnaryOperatorKind ConvertToUnaryOperator(TokenKind kind)
    {
        switch (kind)
        {
            case TokenKind::Bang:
                return UnaryOperatorKind::LogicalNegation;
            case TokenKind::Minus:
                return UnaryOperatorKind::ValueNegation;
                case TokenKind::RefKeyword:
                    return UnaryOperatorKind::ReferenceOf;
            default:
                return UnaryOperatorKind::Invalid;
        }
    }

    UnaryExpression::UnaryExpression(
        const Token& unaryOperatorToken,
        ExpressionUPtr&& expression)
        : Expression(NodeKind::UnaryExpression, expression->type())
        , m_unaryOperatorToken{ unaryOperatorToken }
        , m_expression{ std::move(expression) }
        , m_unaryOperator{ ConvertToUnaryOperator (unaryOperatorToken.kind)}
    {
    }

    std::string stringify(UnaryOperatorKind kind)
    {
        switch (kind)
        {
            case UnaryOperatorKind::LogicalNegation:
                return std::string("LogicalNegation");
            case UnaryOperatorKind::ValueNegation:
                return std::string("ValueNegation");
            case UnaryOperatorKind::ReferenceOf:
                return std::string("ReferenceOf");
            default:
                TODO("Invalid unary operator kind");
        }
        return std::string();
    }
}
