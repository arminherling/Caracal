#include "FunctionCallExpression.h"

namespace Caracal {
    FunctionCallExpression::FunctionCallExpression(
        NameExpressionUPtr&& nameExpression,
        ArgumentsNodeUPtr&& argumentsNode)
        : Expression(NodeKind::FunctionCallExpression, Type::Undefined())
        , m_nameExpression{ std::move(nameExpression) }
        , m_argumentsNode{ std::move(argumentsNode) }
    {
    }
}
