#pragma once

#include <Caracal/API.h>
#include <string>

namespace Caracal
{
    enum class AnnotationKind
    {
        Error,

        Extern,
        Flag,
        Step,
        Builtin,
    };

    [[nodiscard]] CARACAL_API std::string stringify(AnnotationKind kind);
}
