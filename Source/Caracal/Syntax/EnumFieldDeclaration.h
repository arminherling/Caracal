#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/Node.h>
#include <Caracal/Syntax/NameExpression.h>
#include <Caracal/Syntax/Token.h>

namespace Caracal
{
    class CARACAL_API EnumFieldDeclaration : public Node
    {
    public:
        EnumFieldDeclaration(
            const Token& nameToken,
            std::string_view name);

        EnumFieldDeclaration(
            const Token& nameToken,
            std::string_view name,
            const Token& colon1,
            const Token& colon2,
            ExpressionUPtr&& valueExpression);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(EnumFieldDeclaration)

        [[nodiscard]] const Token& nameToken() const noexcept { return m_nameToken; }
        [[nodiscard]] const std::string& name() const noexcept { return m_name; }
        [[nodiscard]] const std::optional<Token>& colon1() const noexcept { return m_colon1; }
        [[nodiscard]] const std::optional<Token>& colon2() const noexcept { return m_colon2; }
        [[nodiscard]] const std::optional<ExpressionUPtr>& valueExpression() const noexcept { return m_valueExpression; }

    private:
        Token m_nameToken;
        std::string m_name;
        std::optional<Token> m_colon1;
        std::optional<Token> m_colon2;
        std::optional<ExpressionUPtr> m_valueExpression;
    };

    using EnumFieldDeclarationUPtr = std::unique_ptr<EnumFieldDeclaration>;
}
