#include "FunctionCallExpression.h"

namespace Caracal {
    FunctionCallExpression::FunctionCallExpression(
        NameExpressionUPtr&& nameExpression,
        ArgumentsNodeUPtr&& argumentsNode)
        : Expression(NodeKind::FunctionCallExpression, Type::Undefined())
        , m_nameExpression{ std::move(nameExpression) }
        , m_argumentsNode{ std::move(argumentsNode) }
        , m_functionType{ Type::Undefined() }
    {
    }

    SourceLocation FunctionCallExpression::sourceLocation(const TokenBuffer& tokens) const
    {
        const auto nameLocation = m_nameExpression->sourceLocation(tokens);
        const auto closeLocation = tokens.getSourceLocation(m_argumentsNode->closeParenthesisToken());
        return SourceLocation{ nameLocation.startIndex, closeLocation.endIndex };
    }
}
