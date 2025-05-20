#pragma once

#include <Compiler/API.h>
#include <Syntax/Expression.h>
#include <Syntax/Token.h>

namespace Caracal
{
    class COMPILER_API ErrorExpression : public Expression
    {
    public:
        explicit ErrorExpression(const Token& token);

        [[nodiscard]] const Token& token() const noexcept { return m_token; }

    private:
        Token m_token;
    };
}
