#pragma once

#include <Compiler/API.h>
#include <Syntax/Statement.h>
#include <Syntax/Token.h>

namespace Caracal
{
    class COMPILER_API SkipStatement : public Statement
    {
    public:
        SkipStatement(
            const Token& keywordToken,
            const Token& semicolonToken);

        SkipStatement(const SkipStatement&) = delete;
        SkipStatement& operator=(const SkipStatement&) = delete;
        
        SkipStatement(SkipStatement&&) = default;
        SkipStatement& operator=(SkipStatement&&) = default;

        [[nodiscard]] const Token& keywordToken() const noexcept { return m_keywordToken; }
        [[nodiscard]] const Token& semicolonToken() const noexcept { return m_semicolonToken; }

    private:
        Token m_keywordToken;
        Token m_semicolonToken;
    };
}
