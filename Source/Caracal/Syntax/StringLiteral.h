#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/Token.h>

namespace Caracal
{
    class CARACAL_API StringLiteral : public Expression
    {
    public:
        explicit StringLiteral(const Token& literalToken);

        [[nodiscard]] const Token& literalToken() const noexcept { return m_literalToken; }

    private:
        Token m_literalToken;
    };
}
