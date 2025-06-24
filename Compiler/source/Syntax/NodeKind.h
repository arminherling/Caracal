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
        TypeFieldDeclaration,
        EnumFieldDeclaration,

        CppBlockStatement,
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

        DiscardLiteral,
        BoolLiteral,
        NumberLiteral,
        StringLiteral,

        TypeNameNode,
        MethodNameNode,
        ParametersNode,
        ParameterNode,
        ReturnTypesNode,
        ReturnTypeNode,
        ArgumentsNode,
        BlockNode,
    };

    [[nodiscard]] COMPILER_API QString stringify(NodeKind kind);
}
