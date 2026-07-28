#pragma once

#include <Caracal/API.h>
#include <string>

namespace Caracal
{
    struct CARACAL_API TypeCheckerOptions
    {
        std::string defaultIntegerType;
        std::string defaultFloatingType;
        std::string defaultEnumBaseType;
    };
}
