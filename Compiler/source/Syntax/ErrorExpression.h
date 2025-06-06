#pragma once

#include <Compiler/API.h>
#include <Syntax/Expression.h>
#include <Syntax/Token.h>

namespace Caracal
{
    class COMPILER_API ErrorExpression : public Expression
    {
    public:
        explicit ErrorExpression(const Token& errorToken);

        [[nodiscard]] const Token& errorToken() const noexcept { return m_errorToken; }

    private:
        Token m_errorToken;
    };
}
