#include "MethodDefinitionStatement.h"

namespace Caracal
{
    MethodDefinitionStatement::MethodDefinitionStatement(
        const Token& keywordToken,
        MethodNameNodeUPtr&& methodNameNode,
        ParametersNodeUPtr&& parametersNode,
        ReturnTypesNodeUPtr&& returnTypesNode,
        BlockNodeUPtr&& bodyNode,
        MethodModifier modifier,
        SpecialFunctionType specialFunctionType)
        : Statement(NodeKind::MethodDefinitionStatement, Type::Undefined())
        , m_keywordToken{ keywordToken }
        , m_methodNameNode{ std::move(methodNameNode) }
        , m_parametersNode{ std::move(parametersNode) }
        , m_returnTypesNode{ std::move(returnTypesNode) }
        , m_bodyNode{ std::move(bodyNode) }
        , m_modifier{ modifier }
        , m_specialFunctionType{ specialFunctionType }
    {
    }

    std::string stringify(MethodModifier modifier)
    {
        switch (modifier)
        {
            case MethodModifier::Public:
                return std::string("Public");
            case MethodModifier::Private:
                return std::string("Private");
            case MethodModifier::Static:
                return std::string("Static");
            default:        
                TODO("String for MethodModifier value was not defined yet");
                return std::string();
        }
    }

    std::string stringify(SpecialFunctionType specialFunctionType)
    {
        switch (specialFunctionType)
        {
            case SpecialFunctionType::None:
                return std::string("None");
            case SpecialFunctionType::Constructor:
                return std::string("Constructor");
            case SpecialFunctionType::Destructor:
                return std::string("Destructor");
            default:        
                TODO("String for SpecialFunctionType value was not defined yet");
                return std::string();
        }
    }
}
