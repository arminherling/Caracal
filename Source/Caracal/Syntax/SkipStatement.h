#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Statement.h>
#include <Caracal/Syntax/Token.h>

namespace Caracal
{
    class CARACAL_API SkipStatement : public Statement
    {
    public:
        SkipStatement(
            const Token& keywordToken,
            const Token& semicolonToken);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(SkipStatement)

        [[nodiscard]] const Token& keywordToken() const noexcept { return m_keywordToken; }
        [[nodiscard]] const Token& semicolonToken() const noexcept { return m_semicolonToken; }

    private:
        Token m_keywordToken;
        Token m_semicolonToken;
    };
}
