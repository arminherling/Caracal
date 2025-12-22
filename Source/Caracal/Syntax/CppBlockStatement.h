#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Statement.h>
#include <Caracal/Syntax/Token.h>
#include <vector>

namespace Caracal
{
    class CARACAL_API CppBlockStatement : public Statement
    {
    public:
        CppBlockStatement(const Token& cppKeywordToken, const Token& openBracketToken, const std::vector<Token>& lines, const Token& closeBracketToken);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(CppBlockStatement)

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
