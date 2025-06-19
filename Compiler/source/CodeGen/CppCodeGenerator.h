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
#include <Syntax/ExpressionStatement.h>
#include <Syntax/EnumDefinitionStatement.h>
#include <Syntax/BlockNode.h>
#include <Syntax/IfStatement.h>
#include <Syntax/WhileStatement.h>
#include <Syntax/BreakStatement.h>
#include <Syntax/SkipStatement.h>
#include <Syntax/TypeDefinitionStatement.h>

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

        void generateNode(Node* node)  noexcept;
        void generateConstantDeclaration(ConstantDeclaration* node) noexcept;
        void generateVariableDeclaration(VariableDeclaration* node) noexcept;
        void generateGlobalDiscardedExpression(Expression* expression) noexcept;
        void generateLocalDiscardedExpression(Expression* expression) noexcept;
        void generateCppBlock(CppBlockStatement* node) noexcept;
        void generateBlockNode(BlockNode* node) noexcept;
        void generateExpressionStatement(ExpressionStatement* node) noexcept;
        void generateAssignmentStatement(AssignmentStatement* node) noexcept;
        QString generateEnumSignature(EnumDefinitionStatement* node) noexcept;
        QString generateFunctionSignature(FunctionDefinitionStatement* node) noexcept;
        void generateTypeDefinitionStatement(TypeDefinitionStatement* node) noexcept;
        void generateFunctionDefinition(FunctionDefinitionStatement* node) noexcept;
        void generateEnumDefinitionStatement(EnumDefinitionStatement* node) noexcept;
        void generateIfStatement(IfStatement* node) noexcept;
        void generateWhileStatement(WhileStatement* node) noexcept;
        void generateBreakStatement(BreakStatement* node) noexcept;
        void generateSkipStatement(SkipStatement* node) noexcept;
        void generateReturnStatement(ReturnStatement* node) noexcept;
        void generateGroupingExpression(GroupingExpression* node) noexcept;
        void generateUnaryExpression(UnaryExpression* node) noexcept;
        void generateBinaryExpression(BinaryExpression* node) noexcept;
        void generateNameExpression(NameExpression* node) noexcept;
        void generateFunctionCallExpression(FunctionCallExpression* node) noexcept;
        void generateBoolLiteral(BoolLiteral* node) noexcept;
        void generateNumberLiteral(NumberLiteral* node) noexcept;
        void generateStringLiteral(StringLiteral* node) noexcept;

        void generateBuiltinPrintFunction(ArgumentsNode* node) noexcept;

        QStringView getCppNameForType(TypeNameNode* typeName) noexcept;

        ParseTree& m_parseTree;
        QStringList m_cppIncludes;
        QStringList m_forwardDeclarations;
        Scope m_currentScope;
        i32 m_discardCount;
        NodeKind m_currentStatement;
    };

    COMPILER_API QString generateCpp(ParseTree& parseTree) noexcept;
}
