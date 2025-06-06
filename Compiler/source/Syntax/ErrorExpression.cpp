#include "ErrorExpression.h"

namespace Caracal
{
    ErrorExpression::ErrorExpression(const Token& errorToken)
        : Expression(NodeKind::Error, Type::Undefined())
        , m_errorToken{ errorToken }
    {
    }
}
