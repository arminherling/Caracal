#include "MethodDefinitionStatement.h"

namespace Caracal 
{
    MethodDefinitionStatement::MethodDefinitionStatement(
        const Token& keywordToken, 
        NameExpressionUPtr&& nameExpression, 
        ParametersNodeUPtr&& parametersNode, 
        ReturnTypesNodeUPtr&& returnTypesNode, 
        BlockNodeUPtr&& bodyNode, 
        MethodModifier modifier)
        : Statement(NodeKind::MethodDefinitionStatement, Type::Undefined())
        , m_keywordToken(keywordToken)
        , m_nameExpression(std::move(nameExpression))
        , m_parametersNode(std::move(parametersNode))
        , m_returnTypesNode(std::move(returnTypesNode))
        , m_bodyNode(std::move(bodyNode))
        , m_modifier(modifier)
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
                return QStringLiteral("Unknown");
        }
    }
}
