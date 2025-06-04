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
        void generateCppBlock(CppBlockStatement* node);
        void generateAssignmentStatement(AssignmentStatement* node);
        void generateFunctionDefinition(FunctionDefinitionStatement* node);
        void generateReturnStatement(ReturnStatement* node);
        void generateBinaryExpression(BinaryExpression* node);
        void generateBoolLiteral(BoolLiteral* node);
        void generateNumberLiteral(NumberLiteral* node);
        void generateStringLiteral(StringLiteral* node);

        ParseTree& m_parseTree;
        QStringList m_cppIncludes;
        Scope m_currentScope;
    };

    COMPILER_API QString generateCpp(ParseTree& parseTree) noexcept;
}
