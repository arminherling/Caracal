//#pragma once
//
//#include <Compiler/API.h>
//#include <Syntax/Expression.h>
//#include <Syntax/Token.h>
//
//class COMPILER_API MemberAccessExpression : public Expression
//{
//public:
//    MemberAccessExpression(
//        const Token& dot, 
//        Expression* expression);
//
//    [[nodiscard]] const Token& dot() const noexcept { return m_dot; }
//    [[nodiscard]] Expression* expression() const noexcept { return m_expression; }
//
//private:
//    Token m_dot;
//    Expression* m_expression;
//};
