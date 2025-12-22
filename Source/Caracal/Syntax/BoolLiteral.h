#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/Token.h>

namespace Caracal
{
    class CARACAL_API BoolLiteral : public Expression
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
