#pragma once

#include <Compiler/API.h>
#include <Debug/BasePrinter.h>
#include <Defines.h>
#include <Syntax/ParseTree.h>
#include <Syntax/CppBlockStatement.h>
#include <Syntax/FunctionDefinitionStatement.h>
#include <Syntax/ReturnStatement.h>
#include <Syntax/BoolLiteral.h>
#include <Syntax/NumberLiteral.h>
#include <Syntax/StringLiteral.h>
#include <QStringList>
#include <Syntax/ConstantDeclaration.h>
#include <Syntax/VariableDeclaration.h>
#include <Syntax/AssignmentStatement.h>
#include <Syntax/BinaryExpression.h>
#include <Syntax/DiscardLiteral.h>
#include <Syntax/NameExpression.h>
#include <Syntax/UnaryExpression.h>
#include <Syntax/GroupingExpression.h>
#include <Syntax/FunctionCallExpression.h>

namespace Caracal
{
    class COMPILER_API CppCodeGenerator: public BasePrinter
    {
    public:
        CppCodeGenerator(ParseTree& parseTree, i32 indentation = 4);

        [[nodiscard]] QString generate();

    private:
        enum class Scope
        {
            Global,
            Function
        };

        void generateNode(Node* node);
        void generateConstantDeclaration(ConstantDeclaration* node);
        void generateVariableDeclaration(VariableDeclaration* node);
        void generateGlobalDiscardedExpression(Expression* expression);
        void generateLocalDiscardedExpression(Expression* expression);
        void generateCppBlock(CppBlockStatement* node);
        void generateAssignmentStatement(AssignmentStatement* node);
        void generateFunctionDefinition(FunctionDefinitionStatement* node);
        void generateReturnStatement(ReturnStatement* node);
        void generateGroupingExpression(GroupingExpression* node);
        void generateUnaryExpression(UnaryExpression* node);
        void generateBinaryExpression(BinaryExpression* node);
        void generateNameExpression(NameExpression* node);
        void generateFunctionCallExpression(FunctionCallExpression* node);
        void generateBoolLiteral(BoolLiteral* node);
        void generateNumberLiteral(NumberLiteral* node);
        void generateStringLiteral(StringLiteral* node);

        void generateBuiltinPrintFunction(ArgumentsNode* node) noexcept;

        ParseTree& m_parseTree;
        QStringList m_cppIncludes;
        Scope m_currentScope;
        i32 m_discardCount;
    };

    COMPILER_API QString generateCpp(ParseTree& parseTree) noexcept;
}
