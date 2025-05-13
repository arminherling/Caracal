//#pragma once
//
//#include <Compiler/API.h>
//#include <Syntax/Expression.h>
//#include <Syntax/Node.h>
//#include <Syntax/Token.h>
//#include <QList>
//
//class COMPILER_API ArgumentsNode : public Node
//{
//public:
//    ArgumentsNode(
//        const Token& openParenthesis,
//        const QList<Expression*>& arguments,
//        const Token& closeParenthesis);
//
//    [[nodiscard]] const Token& openParenthesis() const noexcept { return m_openParenthesis; }
//    [[nodiscard]] const QList<Expression*>& arguments() const noexcept { return m_arguments; }
//    [[nodiscard]] const Token& closeParenthesis() const noexcept { return m_closeParenthesis; }
//
//private:
//    Token m_openParenthesis;
//    QList<Expression*> m_arguments;
//    Token m_closeParenthesis;
//};
