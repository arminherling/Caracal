#include "TypeDefinitionStatement.h"

namespace Caracal
{
    TypeDefinitionStatement::TypeDefinitionStatement(
        const Token& typeKeyword, 
        const Token& nameToken,
        std::string_view name,
        std::optional<ParametersNodeUPtr>&& constructorParameters,
        BlockNodeUPtr&& bodyNode)
        : Statement(NodeKind::TypeDefinitionStatement, Type::Undefined())
        , m_typeKeyword(typeKeyword)
        , m_nameToken(nameToken)
        , m_name(name)
        , m_constructorParameters(std::move(constructorParameters))
        , m_bodyNode(std::move(bodyNode))
    {
    }
}
