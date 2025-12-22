#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/Token.h>

namespace Caracal
{
    class CARACAL_API NameExpression : public Expression
    {
    public:
        NameExpression(const Token& nameToken);

        [[nodiscard]] const Token& nameToken() const noexcept { return m_nameToken; }

    private:
        Token m_nameToken;
    };

    using NameExpressionUPtr = std::unique_ptr<NameExpression>;
}
