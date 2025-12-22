#pragma once

#include <Caracal/API.h>
#include <Caracal/Semantic/Type.h>

namespace Caracal
{
    struct CARACAL_API TypeCheckerOptions
    {
        Type defaultIntegerType;
        Type defaultEnumBaseType;
    };
}
