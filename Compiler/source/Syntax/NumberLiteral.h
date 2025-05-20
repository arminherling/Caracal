#pragma once

#include <Compiler/API.h>
#include <Syntax/Expression.h>
#include <Syntax/Token.h>

namespace Caracal
{
    class COMPILER_API NumberLiteral : public Expression
    {
    public:
        explicit NumberLiteral(const Token& token);

        [[nodiscard]] const Token& token() const noexcept { return m_token; }

    private:
        Token m_token;
    };
}
