#pragma once

#include <Compiler/API.h>
#include <Syntax/Expression.h>
#include <Syntax/Token.h>

namespace Caracal
{
    class COMPILER_API BoolLiteral : public Expression
    {
    public:
        BoolLiteral(const Token& literalToken, bool value);

        [[nodiscard]] const Token& literalToken() const noexcept { return m_literalToken; }
        [[nodiscard]] bool value() const noexcept { return m_value; }

    private:
        Token m_literalToken;
        bool m_value;
    };
}
