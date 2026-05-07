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
        [[nodiscard]] SourceLocation sourceLocation(const TokenBuffer& tokens) const override;

    private:
        Token m_errorToken;
    };
}
