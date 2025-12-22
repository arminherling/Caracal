#pragma once

#include <Caracal/API.h>
#include <string>
#include <filesystem>

namespace Caracal
{
    struct CARACAL_API SourceText
    {
        SourceText(const std::string& text, const std::filesystem::path& filePath = std::filesystem::path());

        std::string text;
        std::filesystem::path filePath;
    };

    using SourceTextSharedPtr = std::shared_ptr<SourceText>;
}
