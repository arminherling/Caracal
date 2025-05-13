#pragma once

#include <Compiler/API.h>
#include <Syntax/Statement.h>
#include <Syntax/Token.h>
#include <vector>

namespace Caracal
{
    class COMPILER_API CppBlockStatement : public Statement
    {
    public:
        CppBlockStatement(const Token& cppKeywordToken, const Token& openBracketToken, const std::vector<Token>& lines, const Token& closeBracketToken);

        CppBlockStatement(const CppBlockStatement&) = delete;
        CppBlockStatement& operator=(const CppBlockStatement&) = delete;

        CppBlockStatement(CppBlockStatement&&) = default;
        CppBlockStatement& operator=(CppBlockStatement&&) = default;

        [[nodiscard]] Token cppKeyword() const noexcept { return m_cppKeywordToken; }
        [[nodiscard]] Token openBracket() const noexcept { return m_openBracketToken; }
        [[nodiscard]] const std::vector<Token>& lines() const noexcept { return m_lines; }
        [[nodiscard]] Token closeBracket() const noexcept { return m_closBracketToken; }

    private:
        Token m_cppKeywordToken;
        Token m_openBracketToken;
        std::vector<Token> m_lines;
        Token m_closBracketToken;
    };
}
