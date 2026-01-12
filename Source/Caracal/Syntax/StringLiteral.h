#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Expression.h>
#include <Caracal/Syntax/Token.h>

namespace Caracal
{
    class CARACAL_API StringLiteral : public Expression
    {
    public:
        StringLiteral(
            const Token& literalToken, 
            const std::string& escapedContent);

        [[nodiscard]] const Token& literalToken() const noexcept { return m_literalToken; }
        [[nodiscard]] const std::string& escapedContent() const noexcept { return m_escapedContent; }

    private:
        Token m_literalToken;
        std::string m_escapedContent;
    };

    using StringLiteralUPtr = std::unique_ptr<StringLiteral>;
}
