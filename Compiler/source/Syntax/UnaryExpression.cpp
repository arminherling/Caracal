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
                //case TokenKind::RefKeyword:
                //    return UnaryOperatornKind::ReferenceOf;
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

    QString stringify(UnaryOperatorKind kind)
    {
        switch (kind)
        {
            case UnaryOperatorKind::LogicalNegation:
                return QStringLiteral("LogicalNegation");
            case UnaryOperatorKind::ValueNegation:
                return QStringLiteral("ValueNegation");
            //case UnaryOperatornKind::ReferenceOf:
            //    return QString("ReferenceOf");
            default:
                TODO("Invalid unary operator kind");
        }
        return QString();
    }
}
