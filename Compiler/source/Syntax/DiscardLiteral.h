#pragma once

#include <Compiler/API.h>
#include <Syntax/Expression.h>
#include <Syntax/Token.h>

namespace Caracal
{
    class COMPILER_API DiscardLiteral : public Expression
    {
    public:
        DiscardLiteral(const Token& underscoreToken);

        [[nodiscard]] const Token& underscoreToken() const noexcept { return m_underscoreToken; }

    private:
        Token m_underscoreToken;
    };
}
