//#pragma once
//
//#include <Compiler/API.h>
//#include <Syntax/Statement.h>
//#include <Syntax/Token.h>
//#include <Syntax/Expression.h>
//
//class COMPILER_API ReturnStatement : public Statement
//{
//public:
//    ReturnStatement(
//        const Token& returnKeyword, 
//        const std::optional<Expression*>& expression);
//
//    [[nodiscard]] std::optional<Expression*> expression() const noexcept { return m_expression; }
//
//private:
//    Token m_returnKeyword;
//    std::optional<Expression*> m_expression;
//};
