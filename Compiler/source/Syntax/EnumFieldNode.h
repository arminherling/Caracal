#pragma once

#include <Compiler/API.h>
#include <Syntax/Node.h>
#include <Syntax/NameExpression.h>
#include <Syntax/Token.h>

namespace Caracal
{
    class COMPILER_API EnumFieldNode : public Node
    {
    public:
        EnumFieldNode(
            NameExpressionUPtr&& nameExpression);

        EnumFieldNode(
            NameExpressionUPtr&& nameExpression,
            const Token& colon1,
            const Token& colon2,
            ExpressionUPtr&& valueExpression);

        EnumFieldNode(const EnumFieldNode&) = delete;
        EnumFieldNode& operator=(const EnumFieldNode&) = delete;

        EnumFieldNode(EnumFieldNode&&) = default;
        EnumFieldNode& operator=(EnumFieldNode&&) = default;
        
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

    using EnumFieldNodeUPtr = std::unique_ptr<EnumFieldNode>;
}
