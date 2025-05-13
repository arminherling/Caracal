//#pragma once
//
//#include <Compiler/API.h>
//#include <Semantic/TypedStatement.h>
//#include <Syntax/TokenBuffer.h>
//
//#include <QList>
//
//class COMPILER_API TypedTree
//{
//public:
//    TypedTree(
//        const TokenBuffer& tokens,
//        const QList<TypedStatement*>& statements);
//
//    [[nodiscard]] QList<TypedStatement*> globalStatements() const noexcept;
//    [[nodiscard]] const TokenBuffer& tokens() noexcept { return m_tokens; }
//
//private:
//    TokenBuffer m_tokens;
//    QList<TypedStatement*> m_statements;
//};
