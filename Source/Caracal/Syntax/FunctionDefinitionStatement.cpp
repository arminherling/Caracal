#include "FunctionDefinitionStatement.h"

namespace Caracal
{
    FunctionDefinitionStatement::FunctionDefinitionStatement(
        const Token& keywordToken, 
        const Token& nameToken,
        std::string_view name,
        ParametersNodeUPtr&& parametersNode,
        ReturnTypesNodeUPtr&& returnTypesNode,
        BlockNodeUPtr&& bodyNode,
        std::vector<AnnotationNodeUPtr>&& annotations)
        : Statement(NodeKind::FunctionDefinitionStatement, Type::Undefined())
        , m_keywordToken{ keywordToken }
        , m_nameToken{ nameToken }
        , m_name{ name }
        , m_parametersNode{ std::move(parametersNode) }
        , m_returnTypesNode{ std::move(returnTypesNode) }
        , m_bodyNode{ std::move(bodyNode) }
        , m_annotations{ std::move(annotations) }
    {
    }

    bool FunctionDefinitionStatement::isExtern() const noexcept
    {
        for (const auto& annotation : m_annotations)
        {
            if (annotation->kind() == AnnotationKind::Extern)
            {
                return true;
            }
        }

        return false;
    }
}
