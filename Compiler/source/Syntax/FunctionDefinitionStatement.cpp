#include "FunctionDefinitionStatement.h"

namespace Caracal
{
    FunctionDefinitionStatement::FunctionDefinitionStatement(
        const Token& keywordToken,
        const Token& nameToken,
        ParametersNodeUPtr&& parametersNode,
        ReturnTypesNodeUPtr&& returnTypesNode,
        BlockNodeUPtr&& bodyNode)
        : Statement(NodeKind::FunctionDefinitionStatement, Type::Undefined())
        , m_keywordToken{ keywordToken }
        , m_nameToken{ nameToken }
        , m_parametersNode{ std::move(parametersNode) }
        , m_returnTypesNode{ std::move(returnTypesNode) }
        , m_bodyNode{ std::move(bodyNode) }
    {
    }
}
