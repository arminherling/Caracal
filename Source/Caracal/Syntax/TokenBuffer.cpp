#include <Caracal/Syntax/TokenBuffer.h>

#include <algorithm>

namespace Caracal
{
    TokenBuffer::TokenBuffer(const SourceTextSharedPtr& source, u16 fileId, i32 firstTokenStart)
        : m_source{ source }
        , m_fileId{ fileId }
        , m_firstTokenStart{ firstTokenStart }
    {
        // one entry per source byte plus the EOF token
        const auto capacity = source->text.size() + 1;
        m_kinds = std::make_unique_for_overwrite<TokenKind[]>(capacity);
        m_locationStorage = std::make_unique_for_overwrite<u8[]>(capacity * sizeof(SourceLocation));
        m_sourceLocations = reinterpret_cast<SourceLocation*>(m_locationStorage.get());
    }

    TokenBuffer::TokenBuffer(const TokenBuffer& other)
        : m_source{ other.m_source }
        , m_fileId{ other.m_fileId }
        , m_firstTokenStart{ other.m_firstTokenStart }
        , m_count{ other.m_count }
    {
        m_kinds = std::make_unique_for_overwrite<TokenKind[]>(m_count);
        m_locationStorage = std::make_unique_for_overwrite<u8[]>(m_count * sizeof(SourceLocation));
        m_sourceLocations = reinterpret_cast<SourceLocation*>(m_locationStorage.get());
        std::copy_n(other.m_kinds.get(), m_count, m_kinds.get());
        std::copy_n(other.m_sourceLocations, m_count, m_sourceLocations);
    }

    TokenBuffer& TokenBuffer::operator=(const TokenBuffer& other)
    {
        if (this != &other)
        {
            *this = TokenBuffer{ other };
        }

        return *this;
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
        return getToken(m_count - 1);
    }

    std::string_view TokenBuffer::getLexeme(const Token& token) const noexcept
    {
        if (token.index < 0 || token.index >= m_count)
            return {};

        const auto& location = m_sourceLocations[token.index];
        return std::string_view(m_source->text).substr(location.startIndex, location.endIndex - location.startIndex);
    }

    const SourceLocation& TokenBuffer::getSourceLocation(const Token& token) const noexcept
    {
        if (token.index < 0 || token.index >= m_count)
        {
            if (m_count > 0)
                return m_sourceLocations[m_count - 1];

            static const SourceLocation fallback{};
            return fallback;
        }

        return m_sourceLocations[token.index];
    }

    std::string_view TokenBuffer::getTrivia(const Token& token) const noexcept
    {
        if (token.index < 0 || token.index >= m_count)
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
