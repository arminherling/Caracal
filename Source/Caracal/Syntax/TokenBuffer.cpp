#include <Caracal/Syntax/TokenBuffer.h>

namespace Caracal
{
    TokenBuffer::TokenBuffer(const SourceTextSharedPtr& source)
        : m_source{ source }
        , m_tokens{}
        , m_lexemes{}
        , m_sourceLocations{}
    {
        const auto initialSize = static_cast<i32>(source->text.size());
        m_tokens.reserve(initialSize);
        m_lexemes.reserve(initialSize);
        m_trivias.reserve(initialSize);
        m_sourceLocations.reserve(initialSize);
    }

    const SourceTextSharedPtr& TokenBuffer::source() const noexcept
    {
        return m_source;
    }

    void TokenBuffer::addToken(const Token& token) noexcept
    {
        m_tokens.push_back(token);
    }

    i32 TokenBuffer::addLexeme(std::string_view lexeme) noexcept
    {
        m_lexemes.push_back(lexeme);
        return m_lexemes.size() - 1;
    }

    i32 TokenBuffer::addTrivia(std::string_view trivia) noexcept
    {
        m_trivias.push_back(trivia);
        return m_trivias.size() - 1;
    }

    i32 TokenBuffer::addSourceLocation(const SourceLocation& sourceLocation) noexcept
    {
        m_sourceLocations.push_back(sourceLocation);
        return m_sourceLocations.size() - 1;
    }

    i32 TokenBuffer::size() const noexcept
    {
        return m_tokens.size();
    }

    const Token& TokenBuffer::getToken(i32 position) const noexcept
    {
        return m_tokens.at(position);
    }

    const Token& TokenBuffer::getLastToken() const noexcept
    {
        return m_tokens.at(m_tokens.size() - 1);
    }

    std::string_view TokenBuffer::getLexeme(const Token& token) const noexcept
    {
        if (token.lexemeIndex < 0 || token.lexemeIndex >= static_cast<i32>(m_lexemes.size()))
            return {};

        return m_lexemes.at(token.lexemeIndex);
    }

    std::string_view TokenBuffer::getTrivia(const Token& token) const noexcept
    {
        if (token.triviaIndex < 0 || token.triviaIndex >= static_cast<i32>(m_trivias.size()))
            return {};

        return m_trivias.at(token.triviaIndex);
    }

    const SourceLocation& TokenBuffer::getSourceLocation(const Token& token) const noexcept
    {
        if (token.locationIndex < 0 || token.locationIndex >= static_cast<i32>(m_sourceLocations.size()))
        {
            if (!m_sourceLocations.empty())
                return m_sourceLocations.back();

            static const SourceLocation fallback{};
            return fallback;
        }

        return m_sourceLocations.at(token.locationIndex);
    }
}
