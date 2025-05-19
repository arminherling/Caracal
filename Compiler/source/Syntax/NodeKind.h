#pragma once

#include <Compiler/API.h>
#include <QString>

namespace Caracal
{
    enum class COMPILER_API NodeKind
    {
        Unknown,
        Error,

        CppBlockStatement,
        FunctionDefinitionStatement,
        BlockNode,

        ParametersNode,
        ParameterNode,

        ReturnTypesNode,
        ReturnTypeNode,

        ReturnStatement
        
        //AssignmentStatement,
        //ExpressionStatement,
        //EnumDefinitionStatement,
        //EnumFieldDefinitionStatement,
        //TypeDefinitionStatement,
        //FieldDefinitionStatement,
        //MethodDefinitionStatement,
        //IfStatement,
        //WhileStatement,

        //UnaryExpression,
        //BinaryExpression,
        //FunctionCallExpression,
        //NameExpression,
        //GroupingExpression,
        //ScopeAccessExpression,
        //MemberAccessExpression,

        //DiscardLiteral,
        //BoolLiteral,
        //NumberLiteral,

        //TypeName,
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
