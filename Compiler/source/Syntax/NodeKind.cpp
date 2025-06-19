#include "NodeKind.h"
#include <Defines.h>
#include <unordered_map>

namespace Caracal
{
    QString stringify(NodeKind kind)
    {
        static const std::unordered_map<NodeKind, QString> kindToString{
            { NodeKind::Unknown,                        QStringLiteral("Unknown") },
            { NodeKind::Error,                          QStringLiteral("Error") },

            { NodeKind::ConstantDeclaration,            QStringLiteral("ConstantDeclaration") },
            { NodeKind::VariableDeclaration,            QStringLiteral("VariableDeclaration") },
            { NodeKind::TypeFieldDeclaration,           QStringLiteral("TypeFieldDeclaration") },

            { NodeKind::CppBlockStatement,              QStringLiteral("CppBlockStatement") },
            { NodeKind::ExpressionStatement,            QStringLiteral("ExpressionStatement") },
            { NodeKind::AssignmentStatement,            QStringLiteral("AssignmentStatement") },
            { NodeKind::TypeDefinitionStatement,        QStringLiteral("TypeDefinitionStatement") },
            { NodeKind::FunctionDefinitionStatement,    QStringLiteral("FunctionDefinitionStatement") },
            { NodeKind::EnumDefinitionStatement,        QStringLiteral("EnumDefinitionStatement") },
            { NodeKind::IfStatement,                    QStringLiteral("IfStatement") },
            { NodeKind::WhileStatement,                 QStringLiteral("WhileStatement") },
            { NodeKind::BreakStatement,                 QStringLiteral("BreakStatement") },
            { NodeKind::SkipStatement,                  QStringLiteral("SkipStatement") },
            { NodeKind::ReturnStatement,                QStringLiteral("ReturnStatement") },

            { NodeKind::GroupingExpression,             QStringLiteral("GroupingExpression") },
            { NodeKind::UnaryExpression,                QStringLiteral("UnaryExpression") },
            { NodeKind::BinaryExpression,               QStringLiteral("BinaryExpression") },
            { NodeKind::NameExpression,                 QStringLiteral("NameExpression") },
            { NodeKind::FunctionCallExpression,         QStringLiteral("FunctionCallExpression") },

            { NodeKind::DiscardLiteral,                 QStringLiteral("DiscardLiteral") },
            { NodeKind::BoolLiteral,                    QStringLiteral("BoolLiteral") },
            { NodeKind::NumberLiteral,                  QStringLiteral("NumberLiteral") },
            { NodeKind::StringLiteral,                  QStringLiteral("StringLiteral") },

            { NodeKind::TypeNameNode,                   QStringLiteral("TypeNameNode") },
            { NodeKind::EnumFieldNode,                  QStringLiteral("EnumFieldNode") },
            { NodeKind::ParametersNode,                 QStringLiteral("ParametersNode") },
            { NodeKind::ParameterNode,                  QStringLiteral("ParameterNode") },
            { NodeKind::ReturnTypesNode,                QStringLiteral("ReturnTypesNode") },
            { NodeKind::ReturnTypeNode,                 QStringLiteral("ReturnTypeNode") },
            { NodeKind::ArgumentsNode,                  QStringLiteral("ArgumentsNode") },
            { NodeKind::BlockNode,                      QStringLiteral("BlockNode") },
        };

        const auto it = kindToString.find(kind);
        if (it != kindToString.end())
            return it->second;

        TODO("String for NodeKind value was not defined yet");
        return QString();
    }
}
