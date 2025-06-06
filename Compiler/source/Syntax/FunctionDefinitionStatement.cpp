#include "FunctionDefinitionStatement.h"

namespace Caracal
{
    FunctionDefinitionStatement::FunctionDefinitionStatement(
        const Token& keywordToken,
        const Token& nameToken,
        ParametersNodeUPtr&& parameters, 
        ReturnTypesNodeUPtr&& returnTypes,
        BlockNodeUPtr&& body)
        : Statement(NodeKind::FunctionDefinitionStatement, Type::Undefined())
        , m_keywordToken{ keywordToken }
        , m_nameToken{ nameToken }
        , m_parameters{ std::move(parameters) }
        , m_returnTypes{ std::move(returnTypes) }
        , m_body{ std::move(body) }
    {
    }
}
