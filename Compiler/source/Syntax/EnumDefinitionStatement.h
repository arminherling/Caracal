//#pragma once
//
//#include <Compiler/API.h>
//#include <Syntax/BlockNode.h>
//#include <Syntax/EnumFieldDefinitionStatement.h>
//#include <Syntax/Statement.h>
//#include <Syntax/Token.h>
//#include <Syntax/TypeName.h>
//
//class COMPILER_API EnumDefinitionStatement : public Statement
//{
//public:
//    EnumDefinitionStatement(
//        const Token& keyword,
//        const Token& name,
//        const std::optional<TypeName>& type,
//        const Token& openBracket,
//        const QList<EnumFieldDefinitionStatement*>& fieldDefinitions,
//        const Token& closeBracket);
//
//    [[nodiscard]] const Token& keyword() const noexcept { return m_keyword; }
//    [[nodiscard]] const Token& name() const noexcept { return m_name; }
//    [[nodiscard]] const std::optional<TypeName>& baseType() const noexcept { return m_baseType; }
//    [[nodiscard]] const Token& openBracket() const noexcept { return m_openBracket; }
//    [[nodiscard]] const QList<EnumFieldDefinitionStatement*>& fieldDefinitions() const noexcept { return m_fieldDefinitions; }
//    [[nodiscard]] const Token& closeBracket() const noexcept { return m_closeBracket; }
//
//private:
//    Token m_keyword;
//    Token m_name;
//    std::optional<TypeName> m_baseType;
//    Token m_openBracket;
//    QList<EnumFieldDefinitionStatement*> m_fieldDefinitions;
//    Token m_closeBracket;
//};
