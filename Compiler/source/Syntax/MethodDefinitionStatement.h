//#pragma once
//
//#include <Compiler/API.h>
//#include <Syntax/Statement.h>
//#include <Syntax/Token.h>
//#include <Syntax/ParametersNode.h>
//#include <Syntax/BlockNode.h>
//
//class COMPILER_API MethodDefinitionStatement : public Statement
//{
//public:
//    MethodDefinitionStatement(
//        const Token& keyword, 
//        const Token& name, 
//        ParametersNode* parameters,
//        BlockNode* body);
//
//    [[nodiscard]] const Token& keyword() const noexcept { return m_keyword; }
//    [[nodiscard]] const Token& name() const noexcept { return m_name; }
//    [[nodiscard]] ParametersNode* parameters()  const noexcept { return m_parameters; }
//    [[nodiscard]] BlockNode* body() const noexcept { return m_body; }
//
//private:
//    Token m_keyword;
//    Token m_name;
//    ParametersNode* m_parameters;
//    BlockNode* m_body;
//};
