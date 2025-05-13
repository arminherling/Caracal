//#pragma once
//
//#include <Compiler/API.h>
//#include <Syntax/Expression.h>
//#include <Syntax/Token.h>
//#include <Syntax/TypeName.h>
//
//class COMPILER_API NumberLiteral : public Expression
//{
//public:
//    NumberLiteral(const Token& token);
//    NumberLiteral(
//        const Token& token, 
//        const Token& colon, 
//        const TypeName& type);
//
//    [[nodiscard]] const Token& token() const noexcept { return m_token; }
//    [[nodiscard]] const std::optional<Token>& colon() const noexcept { return m_colon; }
//    [[nodiscard]] const std::optional<TypeName>& type() const noexcept { return m_type; }
//
//private:
//    Token m_token;
//    std::optional<Token> m_colon;
//    std::optional<TypeName> m_type;
//};
