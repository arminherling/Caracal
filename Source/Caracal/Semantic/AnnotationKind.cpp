#include "AnnotationKind.h"
#include <Caracal/Defines.h>
#include <unordered_map>

namespace Caracal
{
    std::string stringify(AnnotationKind kind)
    {
        static const std::unordered_map<AnnotationKind, std::string_view> kindToString{
            { AnnotationKind::Error,        std::string_view("Error") },

            { AnnotationKind::Extern,       std::string_view("Extern") },
            { AnnotationKind::Flag,         std::string_view("Flag") },
            { AnnotationKind::Step,         std::string_view("Step") },
            { AnnotationKind::Builtin,      std::string_view("Builtin") },
        };

        const auto it = kindToString.find(kind);
        if (it != kindToString.end())
            return std::string(it->second);

        TODO("String for AnnotationKind value was not defined yet");
        return std::string();
    }
}
