#include "SkipStatement.h"

namespace Caracal
{
    SkipStatement::SkipStatement(
        const Token& keywordToken,
        const Token& semicolonToken)
        : Statement(NodeKind::SkipStatement, Type::Undefined())
        , m_keywordToken{ keywordToken }
        , m_semicolonToken{ semicolonToken }
    {
    }
}

