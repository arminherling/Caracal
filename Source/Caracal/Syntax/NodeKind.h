#pragma once

#include <Caracal/API.h>
#include <string>

namespace Caracal
{
    enum class CARACAL_API NodeKind
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
        MemberAccessExpression,

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

    [[nodiscard]] CARACAL_API std::string stringify(NodeKind kind);
}
