#pragma once

#include <Compiler/API.h>
#include <Defines.h>
#include <Syntax/CppBlockStatement.h>
//#include <Syntax/ArgumentsNode.h>
//#include <Syntax/AssignmentStatement.h>
#include <Syntax/BinaryExpression.h>
#include <Syntax/BlockNode.h>
#include <Syntax/EnumDefinitionStatement.h>
#include <Syntax/EnumFieldDeclaration.h>
//#include <Syntax/Error.h>
#include <Syntax/ExpressionStatement.h>
//#include <Syntax/FieldDefinitionStatement.h>
#include <Syntax/ConstantDeclaration.h>
#include <Syntax/VariableDeclaration.h>
#include <Syntax/FunctionDefinitionStatement.h>
#include <Syntax/GroupingExpression.h>
#include <Syntax/IfStatement.h>
//#include <Syntax/MemberAccessExpression.h>
#include <Syntax/MethodDefinitionStatement.h>
#include <Syntax/NameExpression.h>
#include <Syntax/MethodNameNode.h>
#include <Syntax/ParametersNode.h>
#include <Syntax/ParseTree.h>
#include <Syntax/ReturnStatement.h>
#include <Syntax/DiscardLiteral.h>
#include <Syntax/BoolLiteral.h>
#include <Syntax/StringLiteral.h>
#include <Syntax/NumberLiteral.h>
#include <Syntax/AssignmentStatement.h>
//#include <Syntax/TypeDefinitionStatement.h>
//#include <Syntax/TypeName.h>
#include <Syntax/UnaryExpression.h>
#include <Syntax/FunctionCallExpression.h>
#include <Syntax/WhileStatement.h>
#include <Syntax/BreakStatement.h>
#include <Syntax/SkipStatement.h>
#include <Syntax/TypeDefinitionStatement.h>
#include <Syntax/TypeFieldDeclaration.h>
#include <Syntax/MemberAccessExpression.h>
#include <Text/StringBuilder.h>

namespace Caracal 
{
    class COMPILER_API ParseTreePrinter
    {
    public:
        ParseTreePrinter(const ParseTree& parseTree, i32 indentation = 4);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(ParseTreePrinter)

        [[nodiscard]] std::string prettyPrint();

    private:
        void prettyPrintNode(Node* node);
        void prettyPrintCppBlockStatement(CppBlockStatement* node);
        void prettyPrintExpressionStatement(ExpressionStatement* statement);
        void prettyPrintConstantDeclaration(ConstantDeclaration* statement);
        void prettyPrintVariableDeclaration(VariableDeclaration* statement);
        void prettyPrintTypeFieldDeclaration(TypeFieldDeclaration* statement);
        void prettyPrintAssignmentStatement(AssignmentStatement* statement);
        void prettyPrintTypeDefinitionStatement(TypeDefinitionStatement* statement);
        void prettyPrintFunctionDefinitionStatement(FunctionDefinitionStatement* statement);
        void prettyPrintEnumDefinitionStatement(EnumDefinitionStatement* statement);
        void prettyPrintEnumFieldDeclaration(EnumFieldDeclaration* statement);
    //    void PrettyPrintFieldDefinitionStatement(FieldDefinitionStatement* statement);
        void prettyPrintMethodDefinitionStatement(MethodDefinitionStatement* statement);
        void prettyPrintIfStatement(IfStatement* statement);
        void prettyPrintWhileStatement(WhileStatement* statement);
        void prettyPrintBreakStatement(BreakStatement* statement);
        void prettyPrintSkipStatement(SkipStatement* statement);
        void prettyPrintReturnStatement(ReturnStatement* statement);
        void prettyPrintArgumentsNode(ArgumentsNode* arguments);
        void prettyPrintParameterNode(ParameterNode* parameter);
        void prettyPrintParametersNode(ParametersNode* parameters);
        void prettyPrintReturnTypesNode(ReturnTypesNode* returnTypes);
        void prettyPrintTypeNameNode(TypeNameNode* returnType);
        void prettyPrintBlockNode(BlockNode* block);
        void prettyPrintGroupingExpression(GroupingExpression* grouping);
        void prettyPrintUnaryExpression(UnaryExpression* unaryExpression);
        void prettyPrintBinaryExpression(BinaryExpression* binaryExpression);
        void prettyPrintNameExpression(NameExpression* name);
        void prettyPrintMethodNameNode(MethodNameNode* methodName);
        void prettyPrintFunctionCallExpression(FunctionCallExpression* functionCall);
        void prettyPrintMemberAccessExpression(MemberAccessExpression* expression);
        void prettyPrintDiscardLiteral(DiscardLiteral* discard);
        void prettyPrintBoolLiteral(BoolLiteral* node);
        void prettyPrintNumberLiteral(NumberLiteral* number);
        void prettyPrintStringLiteral(StringLiteral* string);
    //    void PrettyPrintMemberAccessExpression(MemberAccessExpression* memberAccess);
    //    void PrettyPrintError(Error* error);

        const ParseTree& m_parseTree;
        StringBuilder m_builder;
    };
}
