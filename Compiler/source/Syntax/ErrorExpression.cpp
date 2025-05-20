#include "ErrorExpression.h"

namespace Caracal
{
    ErrorExpression::ErrorExpression(const Token& token)
        : Expression(NodeKind::Error, Type::Undefined())
        , m_token{ token }
    {
    }
}
