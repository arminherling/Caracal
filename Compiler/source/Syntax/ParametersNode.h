//#pragma once
//
//#include <Compiler/API.h>
//#include <Syntax/Node.h>
//#include <Syntax/ParameterNode.h>
//#include <Syntax/Token.h>
//#include <QList>
//
//class COMPILER_API ParametersNode : public Node
//{
//public:
//    ParametersNode(
//        const Token& openParenthesis,
//        const QList<ParameterNode*>& parameters,
//        const Token& closeParenthesis);
//
//    [[nodiscard]] const Token& openParenthesis() const noexcept { return m_openParenthesis; }
//    [[nodiscard]] const QList<ParameterNode*>& parameters() const noexcept { return m_parameters; }
//    [[nodiscard]] const Token& closeParenthesis() const noexcept { return m_closeParenthesis; }
//
//private:
//    Token m_openParenthesis;
//    const QList<ParameterNode*> m_parameters;
//    Token m_closeParenthesis;
//};
