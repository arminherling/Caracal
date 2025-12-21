#pragma once

#include <Compiler/API.h>
#include <string>
#include <filesystem>

namespace Caracal
{
    struct COMPILER_API SourceText
    {
        SourceText(const std::string& text, const std::filesystem::path& filePath = std::filesystem::path());

        std::string text;
        std::filesystem::path filePath;
    };

    using SourceTextSharedPtr = std::shared_ptr<SourceText>;
}
