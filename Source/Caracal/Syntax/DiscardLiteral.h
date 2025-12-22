#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/Token.h>

namespace Caracal
{
    class CARACAL_API DiscardLiteral : public Expression
    {
    public:
        DiscardLiteral(const Token& underscoreToken);

        [[nodiscard]] const Token& underscoreToken() const noexcept { return m_underscoreToken; }

    private:
        Token m_underscoreToken;
    };
}
