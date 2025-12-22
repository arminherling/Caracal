#include "IfStatement.h"

namespace Caracal 
{
    IfStatement::IfStatement(
        const Token& ifKeyword, 
        ExpressionUPtr&& condition, 
        StatementUPtr&& trueStatement)
        : Statement{ NodeKind::IfStatement, Type::Undefined()}
        , m_ifKeyword{ ifKeyword }
        , m_condition{ std::move(condition) }
        , m_trueStatement{ std::move(trueStatement) }
    {
    }

    IfStatement::IfStatement(
        const Token& ifKeyword,
        ExpressionUPtr&& condition,
        StatementUPtr&& trueStatement,
        const Token& elseKeyword,
        StatementUPtr&& falseStatement)
        : Statement{ NodeKind::IfStatement, Type::Undefined() }
        , m_ifKeyword{ ifKeyword }
        , m_condition{ std::move(condition) }
        , m_trueStatement{ std::move(trueStatement) }
        , m_elseKeyword{ elseKeyword }
        , m_falseStatement{ std::move(falseStatement) }
    {
    }
}
