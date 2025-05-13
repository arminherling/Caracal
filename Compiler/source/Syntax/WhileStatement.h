//#pragma once
//
//#include <Compiler/API.h>
//#include <Syntax/BlockNode.h>
//#include <Syntax/Expression.h>
//#include <Syntax/Statement.h>
//#include <Syntax/Token.h>
//
//class COMPILER_API WhileStatement : public Statement
//{
//public:
//    WhileStatement(
//        const Token& ifKeyword, 
//        Expression* condition, 
//        BlockNode* body);
//
//    [[nodiscard]] Expression* condition() const noexcept { return m_condition; }
//    [[nodiscard]] BlockNode* body() const noexcept { return m_body; }
//
//private:
//    Token m_ifKeyword;
//    Expression* m_condition;
//    BlockNode* m_body;
//};
