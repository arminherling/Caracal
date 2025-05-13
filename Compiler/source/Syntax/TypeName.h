//#pragma once
//
//#include <Compiler/API.h>
//#include <Syntax/Node.h>
//#include <Syntax/NameExpression.h>
//
//class COMPILER_API TypeName : public Node
//{
//public:
//    TypeName(
//        const std::optional<Token>& ref, 
//        NameExpression* name);
//
//    [[nodiscard]] const std::optional<Token>& ref() const noexcept { return m_ref; }
//    [[nodiscard]] bool isReference() const noexcept { return m_ref.has_value(); }
//    [[nodiscard]] NameExpression* name() const noexcept { return m_name; }
//
//private:
//    std::optional<Token> m_ref;
//    NameExpression* m_name;
//};
