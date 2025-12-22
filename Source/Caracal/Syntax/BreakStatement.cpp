#include "BreakStatement.h"

namespace Caracal
{
    BreakStatement::BreakStatement(
        const Token& keywordToken,
        const Token& semicolonToken)
        : Statement(NodeKind::BreakStatement, Type::Undefined())
        , m_keywordToken{ keywordToken }
        , m_semicolonToken{ semicolonToken }
    {
    }
}

