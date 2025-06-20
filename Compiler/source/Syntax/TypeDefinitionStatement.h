#pragma once

#include <Compiler/API.h>
#include <Syntax/BlockNode.h>
#include <Syntax/Statement.h>
#include <Syntax/NameExpression.h>
#include <Syntax/Token.h>

namespace Caracal
{
    class COMPILER_API TypeDefinitionStatement : public Statement
    {
    public:
        TypeDefinitionStatement(
            const Token& typeKeyword, 
            NameExpressionUPtr&& nameExpression,
            BlockNodeUPtr&& bodyNode);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(TypeDefinitionStatement)

        [[nodiscard]] const Token& typeKeyword() const noexcept { return m_typeKeyword; }
        [[nodiscard]] const NameExpressionUPtr& nameExpression() const noexcept { return m_nameExpression; }
        [[nodiscard]] const BlockNodeUPtr& bodyNode() const noexcept { return m_bodyNode; }

    private:
        Token m_typeKeyword;
        NameExpressionUPtr m_nameExpression;
        BlockNodeUPtr m_bodyNode;
    };
}
