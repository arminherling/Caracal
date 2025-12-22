#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/Token.h>

namespace Caracal
{
    class CARACAL_API ErrorExpression : public Expression
    {
    public:
        explicit ErrorExpression(const Token& errorToken);

        [[nodiscard]] const Token& errorToken() const noexcept { return m_errorToken; }

    private:
        Token m_errorToken;
    };
}
