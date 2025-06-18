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
        FunctionDefinitionStatement,
        EnumDefinitionStatement,
        IfStatement,
        WhileStatement,
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
