#include "SourceEncoding.h"

#include <string_view>

namespace Caracal
{
    [[nodiscard]] static std::string DetectUnsupportedEncodingBom(std::string_view source)
    {
        if (source.length() >= 4)
        {
            if (static_cast<u8>(source[0]) == 0xFF && static_cast<u8>(source[1]) == 0xFE
                && static_cast<u8>(source[2]) == 0x00 && static_cast<u8>(source[3]) == 0x00)
            {
                return "UTF-32 LE";
            }

            if (static_cast<u8>(source[0]) == 0x00 && static_cast<u8>(source[1]) == 0x00
                && static_cast<u8>(source[2]) == 0xFE && static_cast<u8>(source[3]) == 0xFF)
            {
                return "UTF-32 BE";
            }
        }

        if (source.length() >= 2)
        {
            if (static_cast<u8>(source[0]) == 0xFF && static_cast<u8>(source[1]) == 0xFE)
            {
                return "UTF-16 LE";
            }

            if (static_cast<u8>(source[0]) == 0xFE && static_cast<u8>(source[1]) == 0xFF)
            {
                return "UTF-16 BE";
            }
        }

        return std::string{};
    }

    [[nodiscard]] static bool IsValidUtf8(std::string_view source, i32 startIndex, i32& invalidIndex, u8& invalidByte)
    {
        auto index = static_cast<size_t>(startIndex);
        while (index < source.length())
        {
            const auto first = static_cast<u8>(source[index]);
            if (first <= 0x7F)
            {
                index++;
                continue;
            }

            i32 continuationCount = 0;
            u8 minimumSecond = 0x80;
            u8 maximumSecond = 0xBF;
            if (first >= 0xC2 && first <= 0xDF)
            {
                continuationCount = 1;
            }
            else if (first == 0xE0)
            {
                continuationCount = 2;
                minimumSecond = 0xA0;
            }
            else if (first == 0xED)
            {
                // surrogate halves are not valid codepoints
                continuationCount = 2;
                maximumSecond = 0x9F;
            }
            else if (first >= 0xE1 && first <= 0xEF)
            {
                continuationCount = 2;
            }
            else if (first == 0xF0)
            {
                continuationCount = 3;
                minimumSecond = 0x90;
            }
            else if (first == 0xF4)
            {
                // codepoints above U+10FFFF do not exist
                continuationCount = 3;
                maximumSecond = 0x8F;
            }
            else if (first >= 0xF1 && first <= 0xF3)
            {
                continuationCount = 3;
            }
            else
            {
                // 0x80-0xC1 cannot start a sequence, 0xF5-0xFF are not UTF-8 at all
                invalidIndex = static_cast<i32>(index);
                invalidByte = first;
                return false;
            }

            for (i32 continuationIndex = 1; continuationIndex <= continuationCount; continuationIndex++)
            {
                if (index + continuationIndex >= source.length())
                {
                    invalidIndex = static_cast<i32>(index);
                    invalidByte = first;
                    return false;
                }

                const auto continuation = static_cast<u8>(source[index + continuationIndex]);
                u8 minimum = 0x80;
                u8 maximum = 0xBF;
                if (continuationIndex == 1)
                {
                    minimum = minimumSecond;
                    maximum = maximumSecond;
                }

                if (continuation < minimum || continuation > maximum)
                {
                    invalidIndex = static_cast<i32>(index + continuationIndex);
                    invalidByte = continuation;
                    return false;
                }
            }

            index += continuationCount + 1;
        }

        return true;
    }

    bool validateSourceEncoding(const SourceTextSharedPtr& sourceText, DiagnosticsBag& diagnostics) noexcept
    {
        const auto source = std::string_view(sourceText->text);
        const auto encodingName = DetectUnsupportedEncodingBom(source);
        if (!encodingName.empty())
        {
            diagnostics.addUnsupportedSourceEncodingError(sourceText, SourceLocation{ 0, 2 }, encodingName);
            return false;
        }

        // the optional UTF-8 BOM is itself valid UTF-8, validation covers the whole file from the start
        i32 invalidIndex = 0;
        u8 invalidByte = 0;
        if (!IsValidUtf8(source, 0, invalidIndex, invalidByte))
        {
            diagnostics.addInvalidUtf8SourceError(sourceText, SourceLocation{ invalidIndex, invalidIndex + 1 }, invalidByte);
            return false;
        }

        return true;
    }
}
