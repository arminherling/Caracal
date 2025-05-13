//#pragma once
//
//#include <Compiler/API.h>
//#include <Syntax/Statement.h>
//#include <Syntax/NameExpression.h>
//#include <Syntax/TypeName.h>
//#include <Syntax/Expression.h>
//
//class COMPILER_API FieldDefinitionStatement : public Statement
//{
//public:
//    FieldDefinitionStatement(
//        NameExpression* name,
//        const std::optional<Token>& colon,
//        const std::optional<TypeName>& type,
//        const std::optional<Token>& equals,
//        const std::optional<Expression*>& expression);
//
//    [[nodiscard]] NameExpression* name() const noexcept { return m_name; }
//    [[nodiscard]] const std::optional<Token>& colon() const noexcept { return m_colon; }
//    [[nodiscard]] const std::optional<TypeName>& type() const noexcept { return m_type; }
//    [[nodiscard]] const std::optional<Token>& equals() const noexcept { return m_equals; }
//    [[nodiscard]] const std::optional<Expression*>& expression() const noexcept { return m_expression; }
//
//private:
//    NameExpression* m_name;
//    std::optional<Token> m_colon;
//    std::optional<TypeName> m_type;
//    std::optional<Token> m_equals;
//    std::optional<Expression*> m_expression;
//};
