#include "NameExpression.h"

namespace Caracal 
{
    NameExpression::NameExpression(const Token& token)
        : Expression(NodeKind::NameExpression, Type::Undefined())
        , m_token{ token }
    {
    }
}
