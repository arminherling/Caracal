//#pragma once
//
//#include <Compiler/API.h>
//#include <Semantic/Type.h>
//#include <Semantic/TypedExpression.h>
//#include <Semantic/TypedStatement.h>
//#include <QList>
//
//class COMPILER_API TypedIfStatement : public TypedStatement
//{
//public:
//    TypedIfStatement(
//        TypedExpression* condition,
//        const QList<TypedStatement*>& body,
//        const std::optional<TypedStatement*>& elseBlock,
//        Node* source,
//        Type type);
//
//    [[nodiscard]] TypedExpression* condition() const noexcept { return m_condition; }
//    [[nodiscard]] const QList<TypedStatement*>& body() const noexcept { return m_body; }
//    [[nodiscard]] std::optional<TypedStatement*> elseBlock() const noexcept { return m_elseBlock; }
//
//private:
//    TypedExpression* m_condition;
//    QList<TypedStatement*> m_body;
//    std::optional<TypedStatement*> m_elseBlock;
//};
