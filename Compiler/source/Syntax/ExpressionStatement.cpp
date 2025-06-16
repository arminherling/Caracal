#include "ExpressionStatement.h"

namespace Caracal 
{
    ExpressionStatement::ExpressionStatement(ExpressionUPtr&& expression, const Token& semicolonToken)
        : Statement(NodeKind::ExpressionStatement, expression->type())
        , m_expression{ std::move(expression) }
        , m_semicolonToken{ semicolonToken }
    {
    }
}
