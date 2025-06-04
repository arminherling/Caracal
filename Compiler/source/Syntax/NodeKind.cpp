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
            { NodeKind::CppBlockStatement,              QStringLiteral("CppBlockStatement") },
            { NodeKind::AssignmentStatement,            QStringLiteral("AssignmentStatement") },
            { NodeKind::FunctionDefinitionStatement,    QStringLiteral("FunctionDefinitionStatement") },
            { NodeKind::ReturnStatement,                QStringLiteral("ReturnStatement") },
            { NodeKind::BinaryExpression,               QStringLiteral("BinaryExpression") },
            { NodeKind::NameExpression,                 QStringLiteral("NameExpression") },
            { NodeKind::DiscardLiteral,                 QStringLiteral("DiscardLiteral") },
            { NodeKind::BoolLiteral,                    QStringLiteral("BoolLiteral") },
            { NodeKind::NumberLiteral,                  QStringLiteral("NumberLiteral") },
            { NodeKind::StringLiteral,                  QStringLiteral("StringLiteral") },
            { NodeKind::ParameterNode,                  QStringLiteral("ParameterNode") },
            { NodeKind::ParametersNode,                 QStringLiteral("ParametersNode") },
            { NodeKind::ReturnTypesNode,                QStringLiteral("ReturnTypesNode") },
            { NodeKind::ReturnTypeNode,                 QStringLiteral("ReturnTypeNode") },
            { NodeKind::BlockNode,                      QStringLiteral("BlockNode") },

            //{ NodeKind::ExpressionStatement,            QStringLiteral("ExpressionStatement") },
            //{ NodeKind::EnumDefinitionStatement,        QStringLiteral("EnumDefinitionStatement") },
            //{ NodeKind::EnumFieldDefinitionStatement,   QStringLiteral("EnumFieldDefinitionStatement") },
            //{ NodeKind::TypeDefinitionStatement,        QStringLiteral("TypeDefinitionStatement") },
            //{ NodeKind::FieldDefinitionStatement,       QStringLiteral("FieldDefinitionStatement") },
            //{ NodeKind::MethodDefinitionStatement,      QStringLiteral("MethodDefinitionStatement") },
            //{ NodeKind::IfStatement,                    QStringLiteral("IfStatement") },
            //{ NodeKind::WhileStatement,                 QStringLiteral("WhileStatement") },
            //{ NodeKind::UnaryExpression,                QStringLiteral("UnaryExpression") },
            //{ NodeKind::FunctionCallExpression,         QStringLiteral("FunctionCallExpression") },
            //{ NodeKind::GroupingExpression,             QStringLiteral("GroupingExpression") },
            //{ NodeKind::ScopeAccessExpression,          QStringLiteral("ScopeAccessExpression") },
            //{ NodeKind::MemberAccessExpression,         QStringLiteral("MemberAccessExpression") },
            //{ NodeKind::TypeName,                       QStringLiteral("TypeName") },
            //{ NodeKind::ArgumentsNode,                  QStringLiteral("ArgumentsNode") },
            //{ NodeKind::TypedAssignmentStatement,       QStringLiteral("TypedAssignmentStatement") },
            //{ NodeKind::TypedExpressionStatement,       QStringLiteral("TypedExpressionStatement") },
            //{ NodeKind::TypedFunctionDefinitionStatement, QStringLiteral("TypedFunctionDefinitionStatement") },
            //{ NodeKind::TypedEnumDefinitionStatement,   QStringLiteral("TypedEnumDefinitionStatement") },
            //{ NodeKind::TypedFieldDefinitionNode,       QStringLiteral("TypedFieldDefinitionNode") },
            //{ NodeKind::TypedTypeDefinitionStatement,   QStringLiteral("TypedTypeDefinitionStatement") },
            //{ NodeKind::TypedMethodDefinitionStatement, QStringLiteral("TypedMethodDefinitionStatement") },
            //{ NodeKind::TypedIfStatement,               QStringLiteral("TypedIfStatement") },
            //{ NodeKind::TypedWhileStatement,            QStringLiteral("TypedWhileStatement") },
            //{ NodeKind::TypedReturnStatement,           QStringLiteral("TypedReturnStatement") },
            //{ NodeKind::TypedReferenceOfExpression,     QStringLiteral("TypedReferenceOfExpression") },
            //{ NodeKind::TypedNegationExpression,        QStringLiteral("TypedNegationExpression") },
            //{ NodeKind::TypedEnumValueAccessExpression, QStringLiteral("TypedEnumValueAccessExpression") },
            //{ NodeKind::TypedFieldAccessExpression,     QStringLiteral("TypedFieldAccessExpression") },
            //{ NodeKind::TypedAdditionExpression,        QStringLiteral("TypedAdditionExpression") },
            //{ NodeKind::TypedSubtractionExpression,     QStringLiteral("TypedSubtractionExpression") },
            //{ NodeKind::TypedMultiplicationExpression,  QStringLiteral("TypedMultiplicationExpression") },
            //{ NodeKind::TypedDivisionExpression,        QStringLiteral("TypedDivisionExpression") },
            //{ NodeKind::TypedFunctionCallExpression,    QStringLiteral("TypedFunctionCallExpression") },
            //{ NodeKind::TypedMethodCallExpression,      QStringLiteral("TypedMethodCallExpression") },
            //{ NodeKind::TypedConstant,                  QStringLiteral("TypedConstant") },
            //{ NodeKind::TypedVariable,                  QStringLiteral("TypedVariable") },
            //{ NodeKind::Discard,                        QStringLiteral("Discard") },
            //{ NodeKind::BoolValue,                      QStringLiteral("BoolValue") },
            //{ NodeKind::U8Value,                        QStringLiteral("U8Value") },
            //{ NodeKind::I32Value,                       QStringLiteral("I32Value") },
            //{ NodeKind::Parameter,                      QStringLiteral("Parameter") },
        };

        const auto it = kindToString.find(kind);
        if (it != kindToString.end())
            return it->second;

        TODO("String for NodeKind value was not defined yet");
        return QString();
    }
}
