#include "TypeDefinitionStatement.h"

namespace Caracal
{
    TypeDefinitionStatement::TypeDefinitionStatement(
        const Token& typeKeyword, 
        const Token& nameToken,
        std::string_view name,
        std::optional<ParametersNodeUPtr>&& constructorParameters,
        BlockNodeUPtr&& bodyNode,
        std::vector<AnnotationNodeUPtr>&& annotations)
        : Statement(NodeKind::TypeDefinitionStatement, Type::Undefined())
        , m_typeKeyword(typeKeyword)
        , m_nameToken(nameToken)
        , m_name(name)
        , m_constructorParameters(std::move(constructorParameters))
        , m_bodyNode(std::move(bodyNode))
        , m_annotations(std::move(annotations))
    {
    }

    bool TypeDefinitionStatement::isBuiltin() const noexcept
    {
        for (const auto& annotation : m_annotations)
        {
            if (annotation->kind() == AnnotationKind::Builtin)
            {
                return true;
            }
        }

        return false;
    }
}
