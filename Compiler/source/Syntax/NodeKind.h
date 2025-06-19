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
        ExpressionStatement,
        AssignmentStatement,
        TypeDefinitionStatement,
        FunctionDefinitionStatement,
        EnumDefinitionStatement,
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
        EnumFieldNode,
        ParametersNode,
        ParameterNode,
        ReturnTypesNode,
        ReturnTypeNode,
        ArgumentsNode,
        BlockNode,
    };

    COMPILER_API [[nodiscard]] QString stringify(NodeKind kind);
}
