#include "NameExpression.h"

namespace Caracal 
{
    NameExpression::NameExpression(const Token& nameToken, std::string_view name)
        : Expression(NodeKind::NameExpression, Type::Undefined())
        , m_nameToken{ nameToken }
        , m_name{ name }
    {
    }

    SourceLocation NameExpression::sourceLocation(const TokenBuffer& tokens) const
    {
        return tokens.getSourceLocation(m_nameToken);
    }
}
