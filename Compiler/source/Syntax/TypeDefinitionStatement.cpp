#include "TypeDefinitionStatement.h"

namespace Caracal
{
    TypeDefinitionStatement::TypeDefinitionStatement(
        const Token& typeKeyword, 
        NameExpressionUPtr&& nameExpression, 
        BlockNodeUPtr&& bodyNode)
        : Statement(NodeKind::TypeDefinitionStatement, Type::Undefined())
        , m_typeKeyword(typeKeyword)
        , m_nameExpression(std::move(nameExpression))
        , m_bodyNode(std::move(bodyNode))
    {
    }
}
