#pragma once

#include <Compiler/API.h>
#include <Syntax/Expression.h>
#include <Syntax/Token.h>

namespace Caracal
{
    class COMPILER_API StringLiteral : public Expression
    {
    public:
        explicit StringLiteral(const Token& literalToken);

        [[nodiscard]] const Token& literalToken() const noexcept { return m_literalToken; }

    private:
        Token m_literalToken;
    };
}
