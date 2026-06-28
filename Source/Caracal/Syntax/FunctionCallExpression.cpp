#include "FunctionCallExpression.h"

namespace Caracal {
    FunctionCallExpression::FunctionCallExpression(
        NameExpressionUPtr&& nameExpression,
        const Token& openParenthesisToken,
        std::vector<Argument>&& arguments,
        const Token& closeParenthesisToken)
        : Expression(NodeKind::FunctionCallExpression, Type::Undefined())
        , m_nameExpression{ std::move(nameExpression) }
        , m_openParenthesisToken{ openParenthesisToken }
        , m_arguments{ std::move(arguments) }
        , m_closeParenthesisToken{ closeParenthesisToken }
        , m_functionType{ Type::Undefined() }
    {
    }

    SourceLocation FunctionCallExpression::sourceLocation(const TokenBuffer& tokens) const
    {
        const auto nameLocation = m_nameExpression->sourceLocation(tokens);
        const auto closeLocation = tokens.getSourceLocation(m_closeParenthesisToken);
        return SourceLocation{ nameLocation.startIndex, closeLocation.endIndex };
    }

    void FunctionCallExpression::setBoundArguments(std::vector<const Expression*> orderedArguments, std::vector<const Expression*> variadicArguments) noexcept
    {
        m_orderedArguments = std::move(orderedArguments);
        m_variadicArguments = std::move(variadicArguments);
    }

    SourceLocation FunctionCallExpression::argumentsLocation(const TokenBuffer& tokens) const
    {
        if (m_arguments.empty())
        {
            const auto openParenthesisLocation = tokens.getSourceLocation(m_openParenthesisToken);
            const auto closeParenthesisLocation = tokens.getSourceLocation(m_closeParenthesisToken);
            return SourceLocation{ openParenthesisLocation.startIndex, closeParenthesisLocation.endIndex };
        }

        const auto firstArgumentLocation = m_arguments.front().value()->sourceLocation(tokens);
        const auto lastArgumentLocation = m_arguments.back().value()->sourceLocation(tokens);
        return SourceLocation{ firstArgumentLocation.startIndex, lastArgumentLocation.endIndex };
    }
}
