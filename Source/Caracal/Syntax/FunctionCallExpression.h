#pragma once

#include <Caracal/API.h>
#include <Caracal/Syntax/NameExpression.h>
#include <Caracal/Syntax/Argument.h>
#include <Caracal/Syntax/Token.h>

#include <vector>

namespace Caracal
{
    class CARACAL_API FunctionCallExpression : public Expression
    {
    public:
        FunctionCallExpression(
            NameExpressionUPtr&& nameExpression,
            const Token& openParenthesisToken,
            std::vector<Argument>&& arguments,
            const Token& closeParenthesisToken);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(FunctionCallExpression)

        [[nodiscard]] const NameExpressionUPtr& nameExpression() const noexcept { return m_nameExpression; }
        [[nodiscard]] const Token& openParenthesisToken() const noexcept { return m_openParenthesisToken; }
        [[nodiscard]] const std::vector<Argument>& arguments() const noexcept { return m_arguments; }
        [[nodiscard]] const Token& closeParenthesisToken() const noexcept { return m_closeParenthesisToken; }
        [[nodiscard]] Type functionType() const noexcept { return m_functionType; }
        void setFunctionType(Type functionType) noexcept { m_functionType = functionType; }
        [[nodiscard]] SourceLocation sourceLocation(const TokenBuffer& tokens) const override;
        [[nodiscard]] SourceLocation argumentsLocation(const TokenBuffer& tokens) const;
        void setBoundArguments(std::vector<const Expression*> orderedArguments, std::vector<const Expression*> variadicArguments) noexcept;
        [[nodiscard]] const std::vector<const Expression*>& orderedArguments() const noexcept { return m_orderedArguments; }
        [[nodiscard]] const std::vector<const Expression*>& variadicArguments() const noexcept { return m_variadicArguments; }

    private:
        NameExpressionUPtr m_nameExpression;
        Token m_openParenthesisToken;
        std::vector<Argument> m_arguments;
        Token m_closeParenthesisToken;
        Type m_functionType;
        std::vector<const Expression*> m_orderedArguments;
        std::vector<const Expression*> m_variadicArguments;
    };
}
