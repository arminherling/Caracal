#pragma once

#include <Compiler/API.h>
#include <Syntax/Expression.h>
#include <Syntax/Token.h>

namespace Caracal
{
    class COMPILER_API DiscardLiteral : public Expression
    {
    public:
        DiscardLiteral(const Token& token);

        [[nodiscard]] const Token& token() const noexcept { return m_token; }

    private:
        Token m_token;
    };
}
