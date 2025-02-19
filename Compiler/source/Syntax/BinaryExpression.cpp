#include "BinaryExpression.h"

BinaryExpression::BinaryExpression(
    Expression* leftExpression, 
    const Token& binaryOperatorToken,
    BinaryOperatornKind binaryOperator,
    Expression* rightExpression)
    : Expression(NodeKind::BinaryExpression)
    , m_leftExpression{ leftExpression }
    , m_binaryOperatorToken{ binaryOperatorToken }
    , m_binaryOperator{ binaryOperator }
    , m_rightExpression{ rightExpression }
{
}

QString StringifyBinaryOperation(BinaryOperatornKind operation)
{
	switch (operation)
	{
		case BinaryOperatornKind::ScopeAccess :
			return QString("Scope Access");
		case BinaryOperatornKind::MemberAccess :
			return QString("Member Access");
		case BinaryOperatornKind::Addition:
			return QString("Addition");
		case BinaryOperatornKind::Subtraction:
			return QString("Subtraction");
		case BinaryOperatornKind::Multiplication:
			return QString("Multiplication");
		case BinaryOperatornKind::Division:
			return QString("Division");
		default:
			TODO("String for BinaryOperatorn value was not defined yet");
	}
	return QString();
}
