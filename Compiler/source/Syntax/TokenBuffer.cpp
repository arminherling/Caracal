#include "TokenBuffer.h"
#include <Syntax/TokenBuffer.h>

TokenBuffer::TokenBuffer(const SourceTextSharedPtr& source)
    : source{source}
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

i32 TokenBuffer::addLexeme(QStringView lexeme) noexcept
{
    lexemes.push_back(lexeme);
    return lexemes.size() - 1;
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

const SourceLocation& TokenBuffer::getSourceLocation(const Token& token) const noexcept
{
    return sourceLocations.at(token.locationIndex);
}

QStringView TokenBuffer::getLexeme(const Token& token) const noexcept
{
    return lexemes.at(token.lexemeIndex);
}
