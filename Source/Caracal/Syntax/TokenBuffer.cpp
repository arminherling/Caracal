#include <Caracal/Syntax/TokenBuffer.h>

namespace Caracal
{
    TokenBuffer::TokenBuffer(const SourceTextSharedPtr& source, u16 fileId, i32 firstTokenStart)
        : m_source{ source }
        , m_fileId{ fileId }
        , m_firstTokenStart{ firstTokenStart }
    {
        const auto initialSize = static_cast<i32>(source->text.size());
        m_kinds.reserve(initialSize);
        m_sourceLocations.reserve(initialSize);
    }

    const SourceTextSharedPtr& TokenBuffer::source() const noexcept
    {
        return m_source;
    }

    u16 TokenBuffer::fileId() const noexcept
    {
        return m_fileId;
    }

    Token TokenBuffer::getLastToken() const noexcept
    {
        return getToken(static_cast<i32>(m_kinds.size()) - 1);
    }

    std::string_view TokenBuffer::getLexeme(const Token& token) const noexcept
    {
        if (token.index < 0 || token.index >= static_cast<i32>(m_sourceLocations.size()))
            return {};

        const auto& location = m_sourceLocations[token.index];
        return std::string_view(m_source->text).substr(location.startIndex, location.endIndex - location.startIndex);
    }

    const SourceLocation& TokenBuffer::getSourceLocation(const Token& token) const noexcept
    {
        if (token.index < 0 || token.index >= static_cast<i32>(m_sourceLocations.size()))
        {
            if (!m_sourceLocations.empty())
                return m_sourceLocations.back();

            static const SourceLocation fallback{};
            return fallback;
        }

        return m_sourceLocations[token.index];
    }

    std::string_view TokenBuffer::getTrivia(const Token& token) const noexcept
    {
        if (token.index < 0 || token.index >= static_cast<i32>(m_sourceLocations.size()))
            return {};

        auto start = m_firstTokenStart;
        if (token.index > 0)
        {
            start = m_sourceLocations[token.index - 1].endIndex;
        }

        const auto end = m_sourceLocations[token.index].startIndex;
        if (end <= start)
            return {};

        return std::string_view(m_source->text).substr(start, end - start);
    }

}
