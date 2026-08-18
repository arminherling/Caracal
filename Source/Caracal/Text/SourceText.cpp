#include <Caracal/Text/SourceText.h>
#include <Caracal/Syntax/LexerSimd.h>

#include <algorithm>
#include <cstring>

namespace Caracal
{
    SourceText::SourceText(
        std::string text_,
        const std::filesystem::path& filePath_)
        : text(std::move(text_))
        , filePath(filePath_)
    {
        // add extra padding for the simd lexer
        text.reserve(text.size() + TailPaddingSize);
        std::memset(text.data() + text.size(), 0, TailPaddingSize);
    }

    const std::vector<i32>& SourceText::lineStarts()
    {
        if (!m_lineStarts.empty())
            return m_lineStarts;

        m_lineStarts.push_back(0);
        const char* base = text.data();
        const char* current = base;
        while (true)
        {
            current += LexerScan::findAnyOf<'\n', '\0'>(current);
            if (static_cast<size_t>(current - base) >= text.size())
                break;

            // an embedded NUL is not a line break, keep scanning past it
            current++;
            if (current[-1] == '\n')
            {
                m_lineStarts.push_back(static_cast<i32>(current - base));
            }
        }

        return m_lineStarts;
    }

    SourceText::LineColumn SourceText::lineColumnAt(i32 offset)
    {
        const auto& starts = lineStarts();
        const auto lineAfter = std::upper_bound(starts.begin(), starts.end(), offset);
        const auto line = static_cast<i32>(lineAfter - starts.begin());
        const auto column = offset - starts[line - 1] + 1;
        return { .line = line, .column = column };
    }
}
