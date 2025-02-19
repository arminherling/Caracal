#include "UnaryExpression.h"

UnaryExpression::UnaryExpression(
    const Token& unaryOperatorToken,
    UnaryOperatornKind unaryOperator,
    Expression* expression)
    : Expression(NodeKind::UnaryExpression)
    , m_unaryOperatorToken { unaryOperatorToken }
    , m_unaryOperator{ unaryOperator }
    , m_expression{ expression }
{
}

QString StringifyUnaryOperation(UnaryOperatornKind kind)
{
    switch (kind)
    {
        case UnaryOperatornKind::Negation:
            return QString("Negation");
        case UnaryOperatornKind::ReferenceOf:
            return QString("ReferenceOf");
        default:
            TODO("String for UnaryOperatorn value was not defined yet");
    }
    return QString();
}
