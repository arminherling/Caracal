//#pragma once
//
//#include <Compiler/API.h>
//#include <Syntax/BlockNode.h>
//#include <Syntax/Statement.h>
//#include <Syntax/Token.h>
//
//class COMPILER_API TypeDefinitionStatement : public Statement
//{
//public:
//    TypeDefinitionStatement(
//        const Token& keyword, 
//        const Token& name, 
//        BlockNode* body);
//
//    [[nodiscard]] const Token& keyword() const noexcept { return m_keyword; }
//    [[nodiscard]] const Token& name() const noexcept { return m_name; }
//    [[nodiscard]] BlockNode* body() const noexcept { return m_body; }
//
//private:
//    Token m_keyword;
//    Token m_name;
//    BlockNode* m_body;
//};
