//#pragma once
//
//#include <Compiler/API.h>
//#include <Semantic/Type.h>
//#include <Semantic/TypedExpression.h>
//#include <Semantic/TypedStatement.h>
//
//class COMPILER_API TypedReturnStatement : public TypedStatement
//{
//public:
//    TypedReturnStatement(
//        const std::optional<TypedExpression*>& expression,
//        Node* source,
//        Type type);
//
//    [[nodiscard]] std::optional<TypedExpression*> expression() const noexcept { return m_expression; }
//
//private:
//    std::optional<TypedExpression*> m_expression;
//};
