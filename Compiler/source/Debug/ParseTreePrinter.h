#pragma once

#include <Compiler/API.h>
#include <Debug/BasePrinter.h>
#include <Defines.h>
#include <Syntax/CppBlockStatement.h>
//#include <Syntax/ArgumentsNode.h>
//#include <Syntax/AssignmentStatement.h>
//#include <Syntax/BinaryExpression.h>
#include <Syntax/BlockNode.h>
//#include <Syntax/BoolLiteral.h>
//#include <Syntax/DiscardLiteral.h>
//#include <Syntax/EnumDefinitionStatement.h>
//#include <Syntax/EnumFieldDefinitionStatement.h>
//#include <Syntax/Error.h>
//#include <Syntax/ExpressionStatement.h>
//#include <Syntax/FieldDefinitionStatement.h>
//#include <Syntax/FunctionCallExpression.h>
#include <Syntax/FunctionDefinitionStatement.h>
//#include <Syntax/GroupingExpression.h>
//#include <Syntax/IfStatement.h>
//#include <Syntax/MemberAccessExpression.h>
//#include <Syntax/MethodDefinitionStatement.h>
//#include <Syntax/NameExpression.h>
//#include <Syntax/NumberLiteral.h>
//#include <Syntax/ParameterNode.h>
#include <Syntax/ParametersNode.h>
#include <Syntax/ParseTree.h>
#include <Syntax/ReturnStatement.h>
#include <Syntax/BoolLiteral.h>
#include <Syntax/StringLiteral.h>
#include <Syntax/NumberLiteral.h>
//#include <Syntax/TypeDefinitionStatement.h>
//#include <Syntax/TypeName.h>
//#include <Syntax/UnaryExpression.h>
//#include <Syntax/WhileStatement.h>
//
namespace Caracal 
{
    class COMPILER_API ParseTreePrinter : public BasePrinter
    {
    public:
        ParseTreePrinter(ParseTree& parseTree, i32 indentation = 4);

        [[nodiscard]] QString prettyPrint();

    private:
        void prettyPrintNode(Node* node);
        void prettyPrintCppBlockStatement(CppBlockStatement* node);
        //    void PrettyPrintAssignmentStatement(AssignmentStatement* statement);
    //    void PrettyPrintExpressionStatement(ExpressionStatement* statement);
        void prettyPrintFunctionDefinitionStatement(FunctionDefinitionStatement* statement);
    //    void PrettyPrintEnumDefinitionStatement(EnumDefinitionStatement* statement);
    //    void PrettyPrintEnumFieldDefinitionStatement(EnumFieldDefinitionStatement* statement);
    //    void PrettyPrintTypeDefinitionStatement(TypeDefinitionStatement* statement);
    //    void PrettyPrintFieldDefinitionStatement(FieldDefinitionStatement* statement);
    //    void PrettyPrintMethodDefinitionStatement(MethodDefinitionStatement* statement);
    //    void PrettyPrintIfStatement(IfStatement* statement);
    //    void PrettyPrintWhileStatement(WhileStatement* statement);
        void prettyPrintReturnStatement(ReturnStatement* statement);
    //    void PrettyPrintArgumentsNode(ArgumentsNode* arguments);
    //    void PrettyPrintParameterNode(ParameterNode* parameter);
        void prettyPrintParametersNode(ParametersNode* parameters);
        void prettyPrintReturnTypesNode(ReturnTypesNode* returnTypes);
        void prettyPrintReturnTypeNode(ReturnTypeNode* returnType);
        void prettyPrintBlockNode(BlockNode* block);
    //
    //    void PrettyPrintDiscardLiteral(DiscardLiteral* discard);
    //    void PrettyPrintFunctionCallExpression(FunctionCallExpression* functionCall);
    //    void PrettyPrintTypeName(const TypeName& type);
        void prettyPrintBoolLiteral(BoolLiteral* node);
    //    void PrettyPrintNameExpression(NameExpression* name);
        void prettyPrintNumberLiteral(NumberLiteral* number);
        void prettyPrintStringLiteral(StringLiteral* string);
    //    void PrettyPrintGroupingExpression(GroupingExpression* grouping);
    //    void PrettyPrintUnaryExpression(UnaryExpression* unaryExpression);
    //    void PrettyPrintBinaryExpression(BinaryExpression* binaryExpression);
    //    void PrettyPrintMemberAccessExpression(MemberAccessExpression* memberAccess);
    //    void PrettyPrintError(Error* error);

        ParseTree& m_parseTree;
    };
}
