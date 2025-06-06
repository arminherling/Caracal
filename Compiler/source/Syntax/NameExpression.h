#pragma once

#include <Compiler/API.h>
#include <Syntax/Expression.h>
#include <Syntax/Token.h>

namespace Caracal
{
    class COMPILER_API NameExpression : public Expression
    {
    public:
        NameExpression(const Token& nameToken);

        [[nodiscard]] const Token& nameToken() const noexcept { return m_nameToken; }

    private:
        Token m_nameToken;
    };

    using NameExpressionUPtr = std::unique_ptr<NameExpression>;
}
