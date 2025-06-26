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

    QString stringify(MethodModifier modifier)
    {
        switch (modifier)
        {
            case MethodModifier::Public:
                return QStringLiteral("Public");
            case MethodModifier::Private:
                return QStringLiteral("Private");
            case MethodModifier::Static:
                return QStringLiteral("Static");
            default:        
                TODO("String for MethodModifier value was not defined yet");
                return QString();
        }
    }

    QString stringify(SpecialFunctionType specialFunctionType)
    {
        switch (specialFunctionType)
        {
            case SpecialFunctionType::None:
                return QStringLiteral("None");
            case SpecialFunctionType::Constructor:
                return QStringLiteral("Constructor");
            case SpecialFunctionType::Destructor:
                return QStringLiteral("Destructor");
            default:        
                TODO("String for SpecialFunctionType value was not defined yet");
                return QString();
        }
    }
}
