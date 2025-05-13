//#pragma once
//
//#include <Compiler/API.h>
//#include <Syntax/Expression.h>
//#include <Syntax/Token.h>
//
//enum class BinaryOperatornKind
//{
//    Invalid,
//    ScopeAccess,
//    MemberAccess,
//    Addition,
//    Subtraction,
//    Multiplication,
//    Division,
//};
//
//class COMPILER_API BinaryExpression : public Expression
//{
//public:
//    BinaryExpression(
//        Expression* leftExpression, 
//        const Token& binaryOperatorToken,
//        BinaryOperatornKind binaryOperator,
//        Expression* rightExpression);
//
//    [[nodiscard]] Expression* leftExpression() const noexcept { return m_leftExpression; }
//    [[nodiscard]] const Token& binaryOperatorToken() const noexcept { return m_binaryOperatorToken; }
//    [[nodiscard]] BinaryOperatornKind binaryOperator() const noexcept { return m_binaryOperator; }
//    [[nodiscard]] Expression* rightExpression() const noexcept { return m_rightExpression; }
//
//private:
//    Expression* m_leftExpression;
//    Token m_binaryOperatorToken;
//    BinaryOperatornKind m_binaryOperator;
//    Expression* m_rightExpression;
//};
//
//COMPILER_API [[nodiscard]] QString StringifyBinaryOperation(BinaryOperatornKind kind);
