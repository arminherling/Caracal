#pragma once

#include <Compiler/API.h>
#include <Debug/BasePrinter.h>
#include <Defines.h>
#include <Syntax/ParseTree.h>

namespace Caracal
{
    class COMPILER_API CppCodeGenerator: public BasePrinter
    {
    public:
        CppCodeGenerator(ParseTree& parseTree, i32 indentation = 4);

        [[nodiscard]] QString generate();

        ParseTree& m_parseTree;
    };

    COMPILER_API QString generateCpp(ParseTree& parseTree) noexcept;
}
