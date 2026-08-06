#pragma once

#include <Caracal/API.h>
#include <Caracal/Defines.h>
#include <string>
#include <filesystem>
#include <vector>

namespace Caracal
{
    struct CARACAL_API SourceText
    {
        // readable NUL bytes guaranteed past text.size() so 16-byte vector loads are safe
        static constexpr i32 TailPaddingSize = 32;

        SourceText(const std::string& text, const std::filesystem::path& filePath = std::filesystem::path());

        struct LineColumn
        {
            i32 line = 0;
            i32 column = 0;
        };

        [[nodiscard]] const std::vector<i32>& lineStarts();
        [[nodiscard]] LineColumn lineColumnAt(i32 offset);

        std::string text;
        std::filesystem::path filePath;

    private:
        std::vector<i32> m_lineStarts;
    };

    using SourceTextSharedPtr = std::shared_ptr<SourceText>;
}
