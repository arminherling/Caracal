#pragma once

#include <Compiler/API.h>
#include <Semantic/Type.h>

namespace Caracal
{
    struct COMPILER_API TypeCheckerOptions
    {
        Type defaultIntegerType;
        Type defaultEnumBaseType;
    };
}
