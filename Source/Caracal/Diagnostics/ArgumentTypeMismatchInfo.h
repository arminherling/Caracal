#pragma once

#include <Caracal/API.h>
#include <Caracal/Text/SourceLocation.h>

#include <string>

namespace Caracal
{
    struct CARACAL_API ArgumentTypeMismatchInfo
    {
        SourceLocation location;
        i32 argumentIndex;
        std::string expectedTypeName;
        std::string actualTypeName;
    };
}
