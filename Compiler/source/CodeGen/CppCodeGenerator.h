#pragma once

#include <Compiler/API.h>
#include <Debug/BasePrinter.h>
#include <Defines.h>
#include <Syntax/ParseTree.h>
#include <Syntax/CppBlockStatement.h>
#include <Syntax/FunctionDefinitionStatement.h>

namespace Caracal
{
    class COMPILER_API CppCodeGenerator: public BasePrinter
    {
    public:
        CppCodeGenerator(ParseTree& parseTree, i32 indentation = 4);

        [[nodiscard]] QString generate();

    private:
        void generateNode(Node* node);
        void generateCppBlock(CppBlockStatement* node);
        void generateFunctionDefinition(FunctionDefinitionStatement* node);

        ParseTree& m_parseTree;
    };

    COMPILER_API QString generateCpp(ParseTree& parseTree) noexcept;
}
