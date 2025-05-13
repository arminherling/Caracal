//#pragma once
//
//#include <Compiler/API.h>
//#include <Syntax/Expression.h>
//#include <Syntax/Token.h>
//
//enum class UnaryOperatornKind
//{
//    Invalid,
//    Negation,
//    ReferenceOf
//};
//
//class COMPILER_API UnaryExpression : public Expression
//{
//public:
//    UnaryExpression(
//        const Token& unaryOperatorToken,
//        UnaryOperatornKind unaryOperator,
//        Expression* expression);
//
//    [[nodiscard]] const Token& unaryOperatorToken() const noexcept { return m_unaryOperatorToken; }
//    [[nodiscard]] UnaryOperatornKind unaryOperator() const noexcept { return m_unaryOperator; }
//    [[nodiscard]] Expression* expression() const noexcept { return m_expression; }
//
//private:
//    Token m_unaryOperatorToken;
//    UnaryOperatornKind m_unaryOperator;
//    Expression* m_expression;
//};
//
//COMPILER_API [[nodiscard]] QString StringifyUnaryOperation(UnaryOperatornKind kind);
