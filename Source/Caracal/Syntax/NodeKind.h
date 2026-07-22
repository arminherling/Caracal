#pragma once

#include <Caracal/API.h>
#include <string>

namespace Caracal
{
    enum class NodeKind
    {
        Unknown,
        Error,

        ConstantDeclaration,
        VariableDeclaration,
        TypeFieldDeclaration,
        EnumFieldDeclaration,

        ExpressionStatement,
        AssignmentStatement,
        FunctionDefinitionStatement,
        EnumDefinitionStatement,
        TypeDefinitionStatement,
        MethodDefinitionStatement,
        IfStatement,
        WhileStatement,
        BreakStatement,
        SkipStatement,
        ReturnStatement,

        GroupingExpression,
        UnaryExpression,
        BinaryExpression,
        NameExpression,
        FunctionCallExpression,
        MemberAccessExpression,

        DiscardLiteral,
        BoolLiteral,
        NumberLiteral,
        StringLiteral,
        ArrayLiteral,

        AnnotationNode,
        TypeNameNode,
        ArrayTypeNameNode,
        MethodNameNode,
        ParametersNode,
        ParameterNode,
        ReturnTypesNode,
        ReturnTypeNode,
        BlockNode,
    };

    [[nodiscard]] CARACAL_API std::string stringify(NodeKind kind);
}
