#include "NameExpression.h"

namespace Caracal 
{
    NameExpression::NameExpression(const Token& nameToken)
        : Expression(NodeKind::NameExpression, Type::Undefined())
        , m_nameToken{ nameToken }
    {
    }
}
