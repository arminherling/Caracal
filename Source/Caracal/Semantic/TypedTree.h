//#pragma once
//
//#include <Caracal/API.h>
//#include <Semantic/TypedStatement.h>
//#include <Caracal/Syntax/TokenBuffer.h>
//
//#include <QList>
//
//class CARACAL_API TypedTree
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
