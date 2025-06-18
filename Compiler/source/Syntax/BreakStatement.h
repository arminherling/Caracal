#pragma once

#include <Compiler/API.h>
#include <Syntax/Statement.h>
#include <Syntax/Token.h>

namespace Caracal
{
    class COMPILER_API BreakStatement : public Statement
    {
    public:
        BreakStatement(
            const Token& keywordToken,
            const Token& semicolonToken);

        BreakStatement(const BreakStatement&) = delete;
        BreakStatement& operator=(const BreakStatement&) = delete;
        
        BreakStatement(BreakStatement&&) = default;
        BreakStatement& operator=(BreakStatement&&) = default;

        [[nodiscard]] const Token& keywordToken() const noexcept { return m_keywordToken; }
        [[nodiscard]] const Token& semicolonToken() const noexcept { return m_semicolonToken; }

    private:
        Token m_keywordToken;
        Token m_semicolonToken;
    };
}
