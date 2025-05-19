#include "FunctionDefinitionStatement.h"

namespace Caracal
{
    FunctionDefinitionStatement::FunctionDefinitionStatement(
        const Token& keyword, 
        const Token& name, 
        ParametersNodeUPtr&& parameters, 
        ReturnTypesNodeUPtr&& returnTypes,
        BlockNodeUPtr&& body)
        : Statement(NodeKind::FunctionDefinitionStatement, Type::Undefined())
        , m_keyword{ keyword }
        , m_name{ name }
        , m_parameters{ std::move(parameters) }
        , m_returnTypes{ std::move(returnTypes) }
        , m_body{ std::move(body) }
    {
    }
}
