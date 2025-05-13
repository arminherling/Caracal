//#pragma once
//
//#include <Compiler/API.h>
//#include <Syntax/Expression.h>
//#include <Syntax/Token.h>
//#include <Syntax/ArgumentsNode.h>
//
//class COMPILER_API FunctionCallExpression : public Expression
//{
//public:
//    FunctionCallExpression(
//        const Token& nameToken, 
//        ArgumentsNode* arguments);
//
//    [[nodiscard]] const Token& name() const noexcept { return m_nameToken; }
//    [[nodiscard]] ArgumentsNode* arguments() const noexcept { return m_arguments; }
//
//private:
//    Token m_nameToken;
//    ArgumentsNode* m_arguments;
//};
