#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/Token.h>

namespace Caracal
{
    class CARACAL_API NameExpression : public Expression
    {
    public:
        NameExpression(const Token& nameToken, std::string_view name);

        [[nodiscard]] const Token& nameToken() const noexcept { return m_nameToken; }
        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        [[nodiscard]] SourceLocation sourceLocation(const TokenBuffer& tokens) const override;

    private:
        Token m_nameToken;
        std::string m_name;
    };

    using NameExpressionUPtr = std::unique_ptr<NameExpression>;
}
