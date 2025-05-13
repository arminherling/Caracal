//#pragma once
//
//#include <Compiler/API.h>
//#include <Syntax/Expression.h>
//#include <Syntax/Token.h>
//
//class COMPILER_API ScopeAccessExpression : public Expression
//{
//public:
//    ScopeAccessExpression(
//        const Token& doubleColon, 
//        Expression* expression);
//
//    [[nodiscard]] const Token& doubleColon() const noexcept { return m_doubleColon; }
//    [[nodiscard]] Expression* expression() const noexcept { return m_expression; }
//
//private:
//    Token m_doubleColon;
//    Expression* m_expression;
//};
