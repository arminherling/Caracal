#include "CppBlockStatement.h"

namespace Caracal
{
    CppBlockStatement::CppBlockStatement(const Token& cppKeywordToken, const Token& openBracketToken, const std::vector<Token>& lines, const Token& closeBracketToken)
        : Statement(NodeKind::CppBlockStatement, Type::Undefined())
        , m_cppKeywordToken{ cppKeywordToken }
        , m_openBracketToken{ openBracketToken }
        , m_lines{ lines }
        , m_closBracketToken{ closeBracketToken }
    {
    }
}
