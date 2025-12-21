#include <Syntax/TokenBuffer.h>

namespace Caracal
{
    TokenBuffer::TokenBuffer(const SourceTextSharedPtr& source)
        : source{ source }
        , tokens{}
        , lexemes{}
        , sourceLocations{}
    {
        const auto initialSize = static_cast<i32>(source->text.size());
        tokens.reserve(initialSize);
        lexemes.reserve(initialSize);
        sourceLocations.reserve(initialSize);
    }

    void TokenBuffer::addToken(const Token& token) noexcept
    {
        tokens.push_back(token);
    }

    i32 TokenBuffer::addLexeme(std::string_view lexeme) noexcept
    {
        lexemes.push_back(lexeme);
        return lexemes.size() - 1;
    }

    i32 TokenBuffer::addTrivia(std::string_view trivia) noexcept
    {
        trivias.push_back(trivia);
        return trivias.size() - 1;
    }

    i32 TokenBuffer::addSourceLocation(const SourceLocation& sourceLocation) noexcept
    {
        sourceLocations.push_back(sourceLocation);
        return sourceLocations.size() - 1;
    }

    i32 TokenBuffer::size() const noexcept
    {
        return tokens.size();
    }

    const Token& TokenBuffer::getToken(i32 position) const noexcept
    {
        return tokens.at(position);
    }

    const Token& TokenBuffer::getLastToken() const noexcept
    {
        return tokens.at(tokens.size() - 1);
    }

    std::string_view TokenBuffer::getLexeme(const Token& token) const noexcept
    {
        return lexemes.at(token.lexemeIndex);
    }

    std::string_view TokenBuffer::getTrivia(const Token& token) const noexcept
    {
        return trivias.at(token.triviaIndex);
    }

    const SourceLocation& TokenBuffer::getSourceLocation(const Token& token) const noexcept
    {
        return sourceLocations.at(token.locationIndex);
    }
}
