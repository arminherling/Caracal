#pragma once

#include <Compiler/API.h>
#include <Syntax/Node.h>
#include <Syntax/NameExpression.h>
#include <Syntax/Token.h>

namespace Caracal
{
    class COMPILER_API EnumFieldDeclaration : public Node
    {
    public:
        EnumFieldDeclaration(
            NameExpressionUPtr&& nameExpression);

        EnumFieldDeclaration(
            NameExpressionUPtr&& nameExpression,
            const Token& colon1,
            const Token& colon2,
            ExpressionUPtr&& valueExpression);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(EnumFieldDeclaration)

        [[nodiscard]] const NameExpressionUPtr& nameExpression() const noexcept { return m_nameExpression; }
        [[nodiscard]] const std::optional<Token>& colon1() const noexcept { return m_colon1; }
        [[nodiscard]] const std::optional<Token>& colon2() const noexcept { return m_colon2; }
        [[nodiscard]] const std::optional<ExpressionUPtr>& valueExpression() const noexcept { return m_valueExpression; }

    private:
        NameExpressionUPtr m_nameExpression;
        std::optional<Token> m_colon1;
        std::optional<Token> m_colon2;
        std::optional<ExpressionUPtr> m_valueExpression;
    };

    using EnumFieldDeclarationUPtr = std::unique_ptr<EnumFieldDeclaration>;
}
