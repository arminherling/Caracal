#include "NodeKind.h"
#include <Defines.h>

namespace Caracal
{
    QString stringify(NodeKind kind)
    {
        switch (kind)
        {
            case NodeKind::Unknown:
                return QString("Unknown");
            case NodeKind::Error:
                return QString("Error");

            case NodeKind::CppBlockStatement:
                return QString("CppBlockStatement");
            case NodeKind::ConstantDeclaration:
                return QString("ConstantDeclaration");
            case NodeKind::VariableDeclaration:
                return QString("VariableDeclaration");
            case NodeKind::FunctionDefinitionStatement:
                return QString("FunctionDefinitionStatement");
            case NodeKind::BlockNode:
                return QString("BlockNode");

            case NodeKind::ParameterNode:
                return QString("ParameterNode");
            case NodeKind::ParametersNode:
                return QString("ParametersNode");

            case NodeKind::ReturnTypesNode:
                return QString("ReturnTypesNode");
            case NodeKind::ReturnTypeNode:
                return QString("ReturnTypeNode");

            case NodeKind::ReturnStatement:
                return QString("ReturnStatement");

            case NodeKind::BoolLiteral:
                return QString("BoolLiteral");
            case NodeKind::NumberLiteral:
                return QString("NumberLiteral");
            case NodeKind::StringLiteral:
                return QString("StringLiteral");

                //case NodeKind::AssignmentStatement:
                //    return QString("AssignmentStatement");
                //case NodeKind::ExpressionStatement:
                //    return QString("ExpressionStatement");
                //case NodeKind::EnumDefinitionStatement:
                //    return QString("EnumDefinitionStatement");
                //case NodeKind::EnumFieldDefinitionStatement:
                //    return QString("EnumFieldDefinitionStatement");
                //case NodeKind::TypeDefinitionStatement:
                //    return QString("TypeDefinitionStatement");
                //case NodeKind::FieldDefinitionStatement:
                //    return QString("FieldDefinitionStatement");
                //case NodeKind::MethodDefinitionStatement:
                //    return QString("MethodDefinitionStatement");
                //case NodeKind::IfStatement:
                //    return QString("IfStatement");
                //case NodeKind::WhileStatement:
                //    return QString("WhileStatement");


                //case NodeKind::UnaryExpression:
                //    return QString("UnaryExpression");
                //case NodeKind::BinaryExpression:
                //    return QString("BinaryExpression");
                //case NodeKind::FunctionCallExpression:
                //    return QString("FunctionCallExpression");
                //case NodeKind::NameExpression:
                //    return QString("NameExpression");
                //case NodeKind::GroupingExpression:
                //    return QString("GroupingExpression");
                //case NodeKind::ScopeAccessExpression:
                //    return QString("ScopeAccessExpression");
                //case NodeKind::MemberAccessExpression:
                //    return QString("MemberAccessExpression");

                //case NodeKind::DiscardLiteral:
                //    return QString("DiscardLiteral");
                //case NodeKind::BoolLiteral:
                //    return QString("BoolLiteral");
                //case NodeKind::NumberLiteral:
                //    return QString("NumberLiteral");

                //case NodeKind::TypeName:
                //    return QString("TypeName");
                //case NodeKind::ArgumentsNode:
                //    return QString("ArgumentsNode");
                //case NodeKind::BlockNode:
                //    return QString("BlockNode");

                //case NodeKind::TypedAssignmentStatement:
                //    return QString("TypedAssignmentStatement");
                //case NodeKind::TypedExpressionStatement:
                //    return QString("TypedExpressionStatement");
                //case NodeKind::TypedFunctionDefinitionStatement:
                //    return QString("TypedFunctionDefinitionStatement");
                //case NodeKind::TypedEnumDefinitionStatement:
                //    return QString("TypedEnumDefinitionStatement");
                //case NodeKind::TypedFieldDefinitionNode:
                //    return QString("TypedFieldDefinitionNode");
                //case NodeKind::TypedTypeDefinitionStatement:
                //    return QString("TypedTypeDefinitionStatement");
                //case NodeKind::TypedMethodDefinitionStatement:
                //    return QString("TypedMethodDefinitionStatement");
                //case NodeKind::TypedIfStatement:
                //    return QString("TypedIfStatement");
                //case NodeKind::TypedWhileStatement:
                //    return QString("TypedWhileStatement");
                //case NodeKind::TypedReturnStatement:
                //    return QString("TypedReturnStatement");

                //case NodeKind::TypedReferenceOfExpression:
                //    return QString("TypedReferenceOfExpression");
                //case NodeKind::TypedNegationExpression:
                //    return QString("TypedNegationExpression");
                //case NodeKind::TypedEnumValueAccessExpression:
                //    return QString("TypedEnumValueAccessExpression");
                //case NodeKind::TypedFieldAccessExpression:
                //    return QString("TypedFieldAccessExpression");
                //case NodeKind::TypedAdditionExpression:
                //    return QString("TypedAdditionExpression");
                //case NodeKind::TypedSubtractionExpression:
                //    return QString("TypedSubtractionExpression");
                //case NodeKind::TypedMultiplicationExpression:
                //    return QString("TypedMultiplicationExpression");
                //case NodeKind::TypedDivisionExpression:
                //    return QString("TypedDivisionExpression");
                //case NodeKind::TypedFunctionCallExpression:
                //    return QString("TypedFunctionCallExpression");
                //case NodeKind::TypedMethodCallExpression:
                //    return QString("TypedMethodCallExpression");

                //case NodeKind::TypedConstant:
                //    return QString("TypedConstant");
                //case NodeKind::TypedVariable:
                //    return QString("TypedVariable");
                //case NodeKind::Discard:
                //    return QString("Discard");
                //case NodeKind::BoolValue:
                //    return QString("BoolValue");
                //case NodeKind::U8Value:
                //    return QString("U8Value");
                //case NodeKind::I32Value:
                //    return QString("I32Value");

                //case NodeKind::Parameter:
                //    return QString("Parameter");

            default:
                TODO("String for NodeKind value was not defined yet");
        }
        return QString();
    }
}
