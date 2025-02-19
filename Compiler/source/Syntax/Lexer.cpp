#include <Syntax/Lexer.h>

#include <unordered_map>
#include <QHash>

[[nodiscard]] static auto InitializeTokenSizes() noexcept
{
    return std::unordered_map<TokenKind, i32>{
        { TokenKind::Plus, 1 },
        { TokenKind::Minus, 1 },
        { TokenKind::Star, 1 },
        { TokenKind::Slash, 1 },
        { TokenKind::Dot, 1 },
        { TokenKind::Colon, 1 },
        { TokenKind::DoubleColon, 2 },
        { TokenKind::Comma, 1 },
        { TokenKind::Equal, 1 },
        { TokenKind::Underscore, 1 },
        { TokenKind::OpenParenthesis, 1 },
        { TokenKind::CloseParenthesis, 1 },
        { TokenKind::OpenBracket, 1 },
        { TokenKind::CloseBracket, 1 },
        { TokenKind::EndOfFile, 0 },
    };
}

[[nodiscard]] static auto InitializeKeywords() noexcept
{
    return std::unordered_map<QStringView, TokenKind>{
        { QStringView(u"def"), TokenKind::DefKeyword},
        { QStringView(u"enum") ,TokenKind::EnumKeyword },
        { QStringView(u"type") ,TokenKind::TypeKeyword },
        { QStringView(u"if") ,TokenKind::IfKeyword },
        { QStringView(u"while") ,TokenKind::WhileKeyword },
        { QStringView(u"return") ,TokenKind::ReturnKeyword },
        { QStringView(u"true") ,TokenKind::TrueKeyword },
        { QStringView(u"false") ,TokenKind::FalseKeyword },
        { QStringView(u"ref") ,TokenKind::RefKeyword },
        { QStringView(u"cpp") ,TokenKind::CppKeyword },
    };
}

[[nodiscard]] static auto TokenSize(TokenKind kind) noexcept
{
    static const auto tokenSizes = InitializeTokenSizes();
    if (const auto result = tokenSizes.find(kind); result != tokenSizes.end())
        return result->second;

    return 1;
}

[[nodiscard]] static auto PeekChar(const SourceTextSharedPtr& source, i32& currentIndex, i32 offset) noexcept
{
    const auto charIndex = currentIndex + offset;
    if (charIndex >= source->text.length())
        return QChar(u'\0');

    return source->text[charIndex];
};

[[nodiscard]] static auto IsNumberOrUnderscore(const QChar& c) noexcept
{
    return c.isNumber() || c == QChar(u'_');
}

[[nodiscard]] static auto IsLetterOrUnderscore(const QChar& c) noexcept
{
    return c.isLetter() || c == QChar(u'_');
}

[[nodiscard]] static auto IsLetterOrNumberOrUnderscore(const QChar& c) noexcept
{
    return c.isLetterOrNumber() || c == QChar(u'_');
}

[[nodiscard]] static auto PeekCurrentChar(const SourceTextSharedPtr& source, i32& currentIndex) noexcept { return PeekChar(source, currentIndex, 0); };
[[nodiscard]] static auto PeekNextChar(const SourceTextSharedPtr& source, i32& currentIndex) noexcept { return PeekChar(source, currentIndex, 1); };

static auto AdvanceCurrentIndex(i32& currentIndex, i32& currentColumn) noexcept
{
    currentIndex++;
    currentColumn++;
};

static auto AdvanceCurrentIndexAndResetLine(i32& currentIndex, i32& currentLine, i32& currentColumn) noexcept
{
    currentIndex++;
    currentLine++;
    currentColumn = 1;
};

static auto AddTokenKindAndAdvance(TokenBuffer& tokenBuffer, const SourceTextSharedPtr& source, i32& currentLine, i32& currentIndex, i32& currentColumn, TokenKind tokenKind) noexcept
{
    const auto tokenSize = TokenSize(tokenKind);
    const auto locationIndex = tokenBuffer.addSourceLocation(
        {
            .source = source,
            .startIndex = currentIndex,
            .endIndex = currentIndex + tokenSize,
            .startColumn = currentColumn,
            .endColumn = currentColumn + tokenSize,
            .startLine = currentLine,
            .endLine = currentLine
        });
    AdvanceCurrentIndex(currentIndex, currentColumn);
    return tokenBuffer.addToken({ .kind = tokenKind, .locationIndex = locationIndex });
};

[[nodiscard]] static auto AddLexemeAndAdvance(TokenBuffer& tokenBuffer, const SourceTextSharedPtr& source, i32& currentLine, i32& currentIndex, i32& currentColumn, TokenKind tokenKind, i32 startIndex, i32 startColumn, i32 startLine) noexcept
{
    const auto length = currentIndex - startIndex;
    const auto identifierIndex = tokenBuffer.addLexeme(QStringView(source->text).sliced(startIndex, length));
    const auto locationIndex = tokenBuffer.addSourceLocation(
        {
            .source = source,
            .startIndex = startIndex,
            .endIndex = currentIndex,
            .startColumn = startColumn,
            .endColumn = currentColumn,
            .startLine = startLine,
            .endLine = currentLine
        });
    return tokenBuffer.addToken({ .kind = tokenKind, .lexemeIndex = identifierIndex, .locationIndex = locationIndex });
};

static auto IsKeyword(const QString& source, i32 currentIndex, i32 startIndex) noexcept
{
    static const auto keywords = InitializeKeywords();
    const auto length = currentIndex - startIndex;
    const auto lexeme = QStringView(source).sliced(startIndex, length);

    if (const auto result = keywords.find(lexeme); result != keywords.end())
        return result->second;

    return TokenKind::Unknown;
}

static auto LexIdentifier(TokenBuffer& tokenBuffer, const SourceTextSharedPtr& source, i32& currentLine, i32& currentIndex, i32& currentColumn) noexcept
{
    const auto startIndex = currentIndex;
    const auto startLine = currentLine;
    const auto startColumn = currentColumn;
    while (IsLetterOrNumberOrUnderscore(PeekCurrentChar(source, currentIndex)))
        AdvanceCurrentIndex(currentIndex, currentColumn);

    const auto maybeKeyword = IsKeyword(source->text, currentIndex, startIndex);
    if(maybeKeyword != TokenKind::Unknown)
        return AddTokenKindAndAdvance(tokenBuffer, source, currentLine, currentIndex, currentColumn, maybeKeyword);

    return AddLexemeAndAdvance(tokenBuffer, source, currentLine, currentIndex, currentColumn, TokenKind::Identifier, startIndex, startColumn, startLine);
};

static auto LexNumber(TokenBuffer& tokenBuffer, const SourceTextSharedPtr& source, i32& currentLine, i32& currentIndex, i32& currentColumn) noexcept
{
    const auto startIndex = currentIndex;
    const auto startLine = currentLine;
    const auto startColumn = currentColumn;

    auto current = PeekCurrentChar(source, currentIndex);
    while (current.isNumber() || (current == QChar(u'_') && PeekNextChar(source, currentIndex) != QChar(u'.')))
    {
        AdvanceCurrentIndex(currentIndex, currentColumn);
        current = PeekCurrentChar(source, currentIndex);
    }

    if (current == QChar(u'.') && PeekNextChar(source, currentIndex).isNumber())
    {
        AdvanceCurrentIndex(currentIndex, currentColumn);

        while (IsNumberOrUnderscore(PeekCurrentChar(source, currentIndex)))
            AdvanceCurrentIndex(currentIndex, currentColumn);
    }

    return AddLexemeAndAdvance(tokenBuffer, source, currentLine, currentIndex, currentColumn, TokenKind::Number, startIndex, startColumn, startLine);
};

static auto LexString(TokenBuffer& tokenBuffer, DiagnosticsBag& diagnostics, const SourceTextSharedPtr& source, i32& currentLine, i32& currentIndex, i32& currentColumn) noexcept
{
    const auto startIndex = currentIndex;
    const auto startLine = currentLine;
    const auto startColumn = currentColumn;

    // Consume opening quotation mark
    AdvanceCurrentIndex(currentIndex, currentColumn);
    while (PeekCurrentChar(source, currentIndex) != QChar(u'\"') && PeekCurrentChar(source, currentIndex) != QChar(u'\0'))
        AdvanceCurrentIndex(currentIndex, currentColumn);

    if (PeekCurrentChar(source, currentIndex) == QChar(u'\"'))
    {
        // Consume closing quotation mark
        AdvanceCurrentIndex(currentIndex, currentColumn);
        return AddLexemeAndAdvance(tokenBuffer, source, currentLine, currentIndex, currentColumn, TokenKind::String, startIndex, startColumn, startLine);
    }
    else
    {
        const auto token = AddLexemeAndAdvance(tokenBuffer, source, currentLine, currentIndex, currentColumn, TokenKind::Error, startIndex, startColumn, startLine);
        const auto& location = tokenBuffer.getSourceLocation(token);
        diagnostics.AddError(DiagnosticKind::_0002_UnterminatedString, location);
        return token;
    }
};

[[nodiscard]] TokenBuffer Lex(const SourceTextSharedPtr& source, DiagnosticsBag& diagnostics) noexcept
{
    TokenBuffer tokenBuffer{ static_cast<i32>(source->text.size()) };
    i32 currentIndex = 0;
    i32 currentLine = 1;
    i32 currentColumn = 1;

    while (true)
    {
        auto current = PeekCurrentChar(source, currentIndex);

        switch (current.unicode())
        {
            case u'\r':
            {
                if (PeekNextChar(source, currentIndex) == QChar(u'\n'))
                    AdvanceCurrentIndex(currentIndex, currentColumn);

                AdvanceCurrentIndexAndResetLine(currentIndex, currentLine, currentColumn);
                break;
            }
            case u'\n':
            {
                AdvanceCurrentIndexAndResetLine(currentIndex, currentLine, currentColumn);
                break;
            }
            case u'\t':
            case u' ':
            {
                AdvanceCurrentIndex(currentIndex, currentColumn);
                break;
            }
            case u'\0':
            {
                AddTokenKindAndAdvance(tokenBuffer, source, currentLine, currentIndex, currentColumn, TokenKind::EndOfFile);
                return tokenBuffer;
            }
            case u'+':
            {
                AddTokenKindAndAdvance(tokenBuffer, source, currentLine, currentIndex, currentColumn, TokenKind::Plus);
                break;
            }
            case u'-':
            {
                AddTokenKindAndAdvance(tokenBuffer, source, currentLine, currentIndex, currentColumn, TokenKind::Minus);
                break;
            }
            case u'*':
            {
                AddTokenKindAndAdvance(tokenBuffer, source, currentLine, currentIndex, currentColumn, TokenKind::Star);
                break;
            }
            case u'/':
            {
                AddTokenKindAndAdvance(tokenBuffer, source, currentLine, currentIndex, currentColumn, TokenKind::Slash);
                break;
            }
            case u'.':
            {
                AddTokenKindAndAdvance(tokenBuffer, source, currentLine, currentIndex, currentColumn, TokenKind::Dot);
                break;
            }
            case u':':
            {
                if (PeekNextChar(source, currentIndex) == QChar(u':'))
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentLine, currentIndex, currentColumn, TokenKind::DoubleColon);
                    AdvanceCurrentIndex(currentIndex, currentColumn);
                    break;
                }
                else if (PeekNextChar(source, currentIndex) == QChar(u'='))
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentLine, currentIndex, currentColumn, TokenKind::ColonEqual);
                    AdvanceCurrentIndex(currentIndex, currentColumn);
                    break;
                }

                AddTokenKindAndAdvance(tokenBuffer, source, currentLine, currentIndex, currentColumn, TokenKind::Colon);
                break;
            }
            case u',':
            {
                AddTokenKindAndAdvance(tokenBuffer, source, currentLine, currentIndex, currentColumn, TokenKind::Comma);
                break;
            }
            case u'=':
            {
                AddTokenKindAndAdvance(tokenBuffer, source, currentLine, currentIndex, currentColumn, TokenKind::Equal);
                break;
            }
            case u'(':
            {
                AddTokenKindAndAdvance(tokenBuffer, source, currentLine, currentIndex, currentColumn, TokenKind::OpenParenthesis);
                break;
            }
            case u')':
            {
                AddTokenKindAndAdvance(tokenBuffer, source, currentLine, currentIndex, currentColumn, TokenKind::CloseParenthesis);
                break;
            }
            case u'{':
            {
                AddTokenKindAndAdvance(tokenBuffer, source, currentLine, currentIndex, currentColumn, TokenKind::OpenBracket);
                break;
            }
            case u'}':
            {
                AddTokenKindAndAdvance(tokenBuffer, source, currentLine, currentIndex, currentColumn, TokenKind::CloseBracket);
                break;
            }
            case u'\"':
            {
                LexString(tokenBuffer, diagnostics, source, currentLine, currentIndex, currentColumn);
                break;
            }
            default:
            {
                if (current == QChar(u'_') && !IsLetterOrNumberOrUnderscore(PeekNextChar(source, currentIndex)))
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentLine, currentIndex, currentColumn, TokenKind::Underscore);
                    break;
                }
                else if (IsLetterOrUnderscore(current))
                {
                    LexIdentifier(tokenBuffer, source, currentLine, currentIndex, currentColumn);
                    break;
                }
                else if (current.isNumber())
                {
                    LexNumber(tokenBuffer, source, currentLine, currentIndex, currentColumn);
                    break;
                }

                const auto token = AddTokenKindAndAdvance(tokenBuffer, source, currentLine, currentIndex, currentColumn, TokenKind::Unknown);
                const auto& location = tokenBuffer.getSourceLocation(token);
                diagnostics.AddError(DiagnosticKind::_0001_FoundIllegalCharacter, location);
                break;
            }
        }
    }
}
