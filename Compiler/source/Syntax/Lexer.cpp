#include <Syntax/Lexer.h>

#include <unordered_map>
#include <QHash>

namespace Caracal
{
    [[nodiscard]] static auto InitializeTokenSizes() noexcept
    {
        return std::unordered_map<TokenKind, i32>{
            { TokenKind::Plus,              1 },
            { TokenKind::Minus,             1 },
            { TokenKind::Star,              1 },
            { TokenKind::Slash,             1 },
            { TokenKind::Dot,               1 },
            { TokenKind::Comma,             1 },
            { TokenKind::Colon,             1 },
            { TokenKind::Semicolon,         1 },
            { TokenKind::Underscore,        1 },
            { TokenKind::Equal,             1 },
            { TokenKind::EqualEqual,        2 },
            { TokenKind::Bang,              1 },
            { TokenKind::BangEqual,         2 },
            { TokenKind::LessThan,          1 },
            { TokenKind::LessThanEqual,     2 },
            { TokenKind::GreaterThan,       1 },
            { TokenKind::GreaterThanEqual,  2 },
            { TokenKind::OpenParenthesis,   1 },
            { TokenKind::CloseParenthesis,  1 },
            { TokenKind::OpenBracket,       1 },
            { TokenKind::CloseBracket,      1 },
            { TokenKind::EndOfFile,         0 },
        };
    }

    [[nodiscard]] static auto InitializeKeywords() noexcept
    {
        return std::unordered_map<QStringView, TokenKind>{
            { QStringView(u"def"),      TokenKind::DefKeyword},
            { QStringView(u"enum"),     TokenKind::EnumKeyword },
            { QStringView(u"type"),     TokenKind::TypeKeyword },
            { QStringView(u"if"),       TokenKind::IfKeyword },
            { QStringView(u"else"),     TokenKind::ElseKeyword },
            { QStringView(u"while"),    TokenKind::WhileKeyword },
            { QStringView(u"break"),    TokenKind::BreakKeyword },
            { QStringView(u"skip"),     TokenKind::SkipKeyword },
            { QStringView(u"return"),   TokenKind::ReturnKeyword },
            { QStringView(u"true"),     TokenKind::TrueKeyword },
            { QStringView(u"false"),    TokenKind::FalseKeyword },
            { QStringView(u"and"),      TokenKind::AndKeyword },
            { QStringView(u"or"),       TokenKind::OrKeyword },
            { QStringView(u"ref"),      TokenKind::RefKeyword },
            { QStringView(u"cpp"),      TokenKind::CppKeyword },
        };
    }

    [[nodiscard]] static auto TokenSize(TokenKind kind) noexcept
    {
        static const auto tokenSizes = InitializeTokenSizes();
        if (const auto result = tokenSizes.find(kind); result != tokenSizes.end())
            return result->second;

        return 1;
    }

    [[nodiscard]] static auto IsUnderscoreOrNumber(QChar c) noexcept
    {
        return c == QChar(u'_') || c.isNumber();
    }

    [[nodiscard]] static auto IsUnderscoreOrLetter(QChar c) noexcept
    {
        return c == QChar(u'_') || c.isLetter();
    }

    [[nodiscard]] static auto IsUnderscoreOrLetterOrNumber(QChar c) noexcept
    {
        return c == QChar(u'_') || c.isLetterOrNumber();
    }

    [[nodiscard]] static auto PeekCurrentChar(QStringView source, i32 charIndex) noexcept
    {
        if (charIndex >= source.length())
            return QChar(u'\0');

        return source[charIndex];
    }

    [[nodiscard]] static auto PeekNextChar(QStringView source, i32& currentIndex) noexcept { return PeekCurrentChar(source, currentIndex + 1); }

    static void AddTokenKindAndAdvance(TokenBuffer& tokenBuffer, i32& currentIndex, TokenKind tokenKind) noexcept
    {
        const auto tokenSize = TokenSize(tokenKind);
        const auto locationIndex = tokenBuffer.addSourceLocation(
            {
                .startIndex = currentIndex,
                .endIndex = currentIndex + tokenSize
            });
        currentIndex++;
        tokenBuffer.addToken({ .kind = tokenKind, .locationIndex = locationIndex });
    };

    [[nodiscard]] static void AddKindAndLexeme(TokenBuffer& tokenBuffer, QStringView source, i32 currentIndex, TokenKind tokenKind, i32 startIndex) noexcept
    {
        const auto length = currentIndex - startIndex;
        const auto identifierIndex = tokenBuffer.addLexeme(source.sliced(startIndex, length));
        const auto locationIndex = tokenBuffer.addSourceLocation(
            {
                .startIndex = startIndex,
                .endIndex = currentIndex
            });
        tokenBuffer.addToken({ .kind = tokenKind, .lexemeIndex = identifierIndex, .locationIndex = locationIndex });
    };

    static auto IdentifierKind(QStringView source, i32 currentIndex, i32 startIndex) noexcept
    {
        static const auto keywords = InitializeKeywords();
        const auto length = currentIndex - startIndex;
        const auto lexeme = source.sliced(startIndex, length);

        if (const auto result = keywords.find(lexeme); result != keywords.end())
            return result->second;

        return TokenKind::Identifier;
    }

    static void LexIdentifier(TokenBuffer& tokenBuffer, QStringView source, i32& currentIndex) noexcept
    {
        const auto startIndex = currentIndex;
        while (IsUnderscoreOrLetterOrNumber(PeekCurrentChar(source, currentIndex)))
            currentIndex++;

        const auto maybeKeywordKind = IdentifierKind(source, currentIndex, startIndex);

        AddKindAndLexeme(tokenBuffer, source, currentIndex, maybeKeywordKind, startIndex);
    };

    static void LexNumber(TokenBuffer& tokenBuffer, QStringView source, i32& currentIndex) noexcept
    {
        const auto startIndex = currentIndex;

        auto current = PeekCurrentChar(source, currentIndex);
        while (current.isNumber() || (current == QChar(u'_') && PeekNextChar(source, currentIndex) != QChar(u'.')))
        {
            currentIndex++;
            current = PeekCurrentChar(source, currentIndex);
        }

        if (current == QChar(u'.') && PeekNextChar(source, currentIndex).isNumber())
        {
            currentIndex++;

            while (IsUnderscoreOrNumber(PeekCurrentChar(source, currentIndex)))
                currentIndex++;
        }

        AddKindAndLexeme(tokenBuffer, source, currentIndex, TokenKind::Number, startIndex);
    };

    static void LexString(TokenBuffer& tokenBuffer, DiagnosticsBag& diagnostics, QStringView source, i32& currentIndex) noexcept
    {
        const auto startIndex = currentIndex;

        // Consume opening quotation mark
        currentIndex++;
        auto current = PeekCurrentChar(source, currentIndex);
        while (current != QChar(u'\"') && current != QChar(u'\0'))
        {
            if (current == QChar(u'\\'))
            {
                currentIndex++;
            }
            currentIndex++;
            current = PeekCurrentChar(source, currentIndex);
        }

        if (current == QChar(u'\"'))
        {
            // Consume closing quotation mark
            currentIndex++;
            AddKindAndLexeme(tokenBuffer, source, currentIndex, TokenKind::String, startIndex);
        }
        else
        {
            AddKindAndLexeme(tokenBuffer, source, currentIndex, TokenKind::Error, startIndex);
            const auto& lastToken = tokenBuffer.getLastToken();
            const auto& location = tokenBuffer.getSourceLocation(lastToken);
            diagnostics.AddError(DiagnosticKind::_0002_UnterminatedString, location);
        }
    };

    [[nodiscard]] TokenBuffer lex(const SourceTextSharedPtr& sourceText, DiagnosticsBag& diagnostics) noexcept
    {
        TokenBuffer tokenBuffer{ sourceText };
        const auto source = QStringView(sourceText->text);
        i32 currentIndex = 0;

        while (true)
        {
            const auto current = PeekCurrentChar(source, currentIndex);

            switch (current.unicode())
            {
                case u'\r':
                {
                    if (PeekNextChar(source, currentIndex) == QChar(u'\n'))
                        currentIndex++;

                    currentIndex++;
                    break;
                }
                case u'\n':
                {
                    currentIndex++;
                    break;
                }
                case u'\t':
                case u' ':
                {
                    currentIndex++;
                    break;
                }
                case u'\0':
                {
                    AddTokenKindAndAdvance(tokenBuffer, currentIndex, TokenKind::EndOfFile);
                    return tokenBuffer;
                }
                case u'+':
                {
                    AddTokenKindAndAdvance(tokenBuffer, currentIndex, TokenKind::Plus);
                    break;
                }
                case u'-':
                {
                    AddTokenKindAndAdvance(tokenBuffer, currentIndex, TokenKind::Minus);
                    break;
                }
                case u'*':
                {
                    AddTokenKindAndAdvance(tokenBuffer, currentIndex, TokenKind::Star);
                    break;
                }
                case u'/':
                {
                    AddTokenKindAndAdvance(tokenBuffer, currentIndex, TokenKind::Slash);
                    break;
                }
                case u'.':
                {
                    AddTokenKindAndAdvance(tokenBuffer, currentIndex, TokenKind::Dot);
                    break;
                }
                case u':':
                {
                    AddTokenKindAndAdvance(tokenBuffer, currentIndex, TokenKind::Colon);
                    break;
                }
                case u';':
                {
                    AddTokenKindAndAdvance(tokenBuffer, currentIndex, TokenKind::Semicolon);
                    break;
                }
                case u'\'':
                {
                    AddTokenKindAndAdvance(tokenBuffer, currentIndex, TokenKind::Uptick);
                    break;
                }
                case u',':
                {
                    AddTokenKindAndAdvance(tokenBuffer, currentIndex, TokenKind::Comma);
                    break;
                }
                case u'=':
                {
                    if (PeekNextChar(source, currentIndex) == QChar(u'='))
                    {
                        AddTokenKindAndAdvance(tokenBuffer, currentIndex, TokenKind::EqualEqual);
                        currentIndex++; // advance for the second '='
                    }
                    else
                    {
                        AddTokenKindAndAdvance(tokenBuffer, currentIndex, TokenKind::Equal);
                    }
                    break;
                }
                case u'!':
                {
                    if (PeekNextChar(source, currentIndex) == QChar(u'='))
                    {
                        AddTokenKindAndAdvance(tokenBuffer, currentIndex, TokenKind::BangEqual);
                        currentIndex++; // advance for the '='
                    }
                    else
                    {
                        AddTokenKindAndAdvance(tokenBuffer, currentIndex, TokenKind::Bang);
                    }
                    break;
                }
                case u'<':
                {
                    if (PeekNextChar(source, currentIndex) == QChar(u'='))
                    {
                        AddTokenKindAndAdvance(tokenBuffer, currentIndex, TokenKind::LessThanEqual);
                        currentIndex++; // advance for the '='
                    }
                    else
                    {
                        AddTokenKindAndAdvance(tokenBuffer, currentIndex, TokenKind::LessThan);
                    }
                    break;
                }
                case u'>':
                {
                    if (PeekNextChar(source, currentIndex) == QChar(u'='))
                    {
                        AddTokenKindAndAdvance(tokenBuffer, currentIndex, TokenKind::GreaterThanEqual);
                        currentIndex++; // advance for the '='
                    }
                    else
                    {
                        AddTokenKindAndAdvance(tokenBuffer, currentIndex, TokenKind::GreaterThan);
                    }
                    break;
                }
                case u'(':
                {
                    AddTokenKindAndAdvance(tokenBuffer, currentIndex, TokenKind::OpenParenthesis);
                    break;
                }
                case u')':
                {
                    AddTokenKindAndAdvance(tokenBuffer, currentIndex, TokenKind::CloseParenthesis);
                    break;
                }
                case u'{':
                {
                    AddTokenKindAndAdvance(tokenBuffer, currentIndex, TokenKind::OpenBracket);
                    break;
                }
                case u'}':
                {
                    AddTokenKindAndAdvance(tokenBuffer, currentIndex, TokenKind::CloseBracket);
                    break;
                }
                case u'\"':
                {
                    LexString(tokenBuffer, diagnostics, source, currentIndex);
                    break;
                }
                default:
                {
                    if (current == QChar(u'_') && !IsUnderscoreOrLetterOrNumber(PeekNextChar(source, currentIndex)))
                    {
                        AddTokenKindAndAdvance(tokenBuffer, currentIndex, TokenKind::Underscore);
                        break;
                    }
                    else if (IsUnderscoreOrLetter(current))
                    {
                        LexIdentifier(tokenBuffer, source, currentIndex);
                        break;
                    }
                    else if (current.isNumber())
                    {
                        LexNumber(tokenBuffer, source, currentIndex);
                        break;
                    }

                    AddTokenKindAndAdvance(tokenBuffer, currentIndex, TokenKind::Unknown);
                    const auto& lastToken = tokenBuffer.getLastToken();
                    const auto& location = tokenBuffer.getSourceLocation(lastToken);
                    diagnostics.AddError(DiagnosticKind::_0001_FoundIllegalCharacter, location);
                    break;
                }
            }
        }
    }
}
