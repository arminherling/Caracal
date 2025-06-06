#pragma once

#include <Compiler/API.h>
#include <QString>

namespace Caracal
{
    enum class COMPILER_API NodeKind
    {
        Unknown,
        Error,

        ConstantDeclaration,
        VariableDeclaration,

        CppBlockStatement,
        AssignmentStatement,
        FunctionDefinitionStatement,
        ReturnStatement,

        GroupingExpression,
        UnaryExpression,
        BinaryExpression,
        NameExpression,

        DiscardLiteral,
        BoolLiteral,
        NumberLiteral,
        StringLiteral,

        TypeNameNode,
        ParametersNode,
        ParameterNode,
        ReturnTypesNode,
        ReturnTypeNode,
        BlockNode,

        //ExpressionStatement,
        //EnumDefinitionStatement,
        //EnumFieldDefinitionStatement,
        //TypeDefinitionStatement,
        //FieldDefinitionStatement,
        //MethodDefinitionStatement,
        //IfStatement,
        //WhileStatement,

        //FunctionCallExpression,
        //ScopeAccessExpression,
        //MemberAccessExpression,

        //ArgumentsNode,

        //TypedAssignmentStatement,
        //TypedExpressionStatement,
        //TypedFunctionDefinitionStatement,
        //TypedEnumDefinitionStatement,
        //TypedFieldDefinitionNode,
        //TypedTypeDefinitionStatement,
        //TypedMethodDefinitionStatement,
        //TypedIfStatement,
        //TypedWhileStatement,
        //TypedReturnStatement,

        //TypedReferenceOfExpression,
        //TypedNegationExpression,
        //TypedEnumValueAccessExpression,
        //TypedFieldAccessExpression,
        //TypedAdditionExpression,
        //TypedSubtractionExpression,
        //TypedMultiplicationExpression,
        //TypedDivisionExpression,
        //TypedFunctionCallExpression,
        //TypedMethodCallExpression,

        //TypedConstant,
        //TypedVariable,
        //Discard,
        //BoolValue,
        //U8Value,
        //I32Value,

        //Parameter
    };

    COMPILER_API [[nodiscard]] QString stringify(NodeKind kind);
}
