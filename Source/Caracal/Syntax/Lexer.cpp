#include <Caracal/Syntax/Lexer.h>
#include <Caracal/Profiling.h>

#include <unordered_map>

namespace Caracal
{
    [[nodiscard]] static auto IsLetter(char c) noexcept
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    [[nodiscard]] static auto IsNumber(char c) noexcept
    {
        return (c >= '0' && c <= '9');
    }

    [[nodiscard]] static auto IsUnderscoreOrNumber(char c) noexcept
    {
        return (c == '_') || IsNumber(c);
    }

    [[nodiscard]] static auto IsUnderscoreOrLetter(char c) noexcept
    {
        return (c == '_') || IsLetter(c);
    }

    [[nodiscard]] static auto IsUnderscoreOrLetterOrNumber(char c) noexcept
    {
        return (c == '_') || IsLetter(c) || IsNumber(c);
    }

    static void AddTokenKindAndAdvance(TokenBuffer& tokenBuffer, std::string_view source, const char*& currentIndex, TokenKind tokenKind, i32 tokenSize) noexcept
    {
        const auto startIndex = static_cast<i32>(currentIndex - source.data());
        tokenBuffer.addToken(tokenKind, { .startIndex = startIndex, .endIndex = startIndex + tokenSize });
        currentIndex += tokenSize;
    };

    static void AddTokenFromStart(TokenBuffer& tokenBuffer, std::string_view source, const char* currentIndex, TokenKind tokenKind, i32 startIndex) noexcept
    {
        const auto endIndex = static_cast<i32>(currentIndex - source.data());
        tokenBuffer.addToken(tokenKind, { .startIndex = startIndex, .endIndex = endIndex });
    };

    [[nodiscard]] static auto InitializeKeywords() noexcept
    {
        return std::unordered_map<std::string_view, TokenKind>{
            { std::string_view("def"),      TokenKind::DefKeyword},
            { std::string_view("enum"),     TokenKind::EnumKeyword },
            { std::string_view("type"),     TokenKind::TypeKeyword },
            { std::string_view("if"),       TokenKind::IfKeyword },
            { std::string_view("else"),     TokenKind::ElseKeyword },
            { std::string_view("while"),    TokenKind::WhileKeyword },
            { std::string_view("break"),    TokenKind::BreakKeyword },
            { std::string_view("skip"),     TokenKind::SkipKeyword },
            { std::string_view("return"),   TokenKind::ReturnKeyword },
            { std::string_view("true"),     TokenKind::TrueKeyword },
            { std::string_view("false"),    TokenKind::FalseKeyword },
            { std::string_view("and"),      TokenKind::AndKeyword },
            { std::string_view("or"),       TokenKind::OrKeyword },
            { std::string_view("ref"),      TokenKind::RefKeyword },
        };
    }

    static TokenKind IdentifierKind(const char* start, i32 length) noexcept
    {
        static const auto keywords = InitializeKeywords();
        const auto lexeme = std::string_view(start, static_cast<size_t>(length));

        if (const auto result = keywords.find(lexeme); result != keywords.end())
            return result->second;

        return TokenKind::Identifier;
    }

    static void LexIdentifier(TokenBuffer& tokenBuffer, std::string_view source, const char*& currentIndex) noexcept
    {
        const char* start = currentIndex;
        while (IsUnderscoreOrLetterOrNumber(*currentIndex))
            currentIndex++;

        const auto startIndex = static_cast<i32>(start - source.data());
        const auto maybeKeywordKind = IdentifierKind(start, static_cast<i32>(currentIndex - start));

        AddTokenFromStart(tokenBuffer, source, currentIndex, maybeKeywordKind, startIndex);
    };

    static void LexNumber(TokenBuffer& tokenBuffer, std::string_view source, const char*& currentIndex) noexcept
    {
        const auto startIndex = static_cast<i32>(currentIndex - source.data());

        // checking the char after underscore is safe, it is EOF terminator in the worst case
        while (IsNumber(*currentIndex) || (*currentIndex == '_' && currentIndex[1] != '.'))
            currentIndex++;

        if (*currentIndex == '.' && IsNumber(currentIndex[1]))
        {
            currentIndex++;

            while (IsUnderscoreOrNumber(*currentIndex))
                currentIndex++;
        }

        AddTokenFromStart(tokenBuffer, source, currentIndex, TokenKind::Number, startIndex);
    };

    static void LexString(
        TokenBuffer& tokenBuffer,
        DiagnosticsBag& diagnostics,
        const SourceTextSharedPtr& sourceText,
        std::string_view source,
        const char*& currentIndex) noexcept
    {
        const auto startIndex = static_cast<i32>(currentIndex - source.data());

        // Consume opening quotation mark
        currentIndex++;
        while (*currentIndex != '"' && *currentIndex != '\r' && *currentIndex != '\n' && *currentIndex != '\0')
        {
            // backslash consumes the next char unless we are at EOF
            if (*currentIndex == '\\' && currentIndex[1] != '\0')
            {
                currentIndex++;
            }
            currentIndex++;
        }

        if (*currentIndex == '\"')
        {
            // Consume closing quotation mark
            currentIndex++;
            AddTokenFromStart(tokenBuffer, source, currentIndex, TokenKind::String, startIndex);
        }
        else
        {
            AddTokenFromStart(tokenBuffer, source, currentIndex, TokenKind::Error, startIndex);
            const auto& lastToken = tokenBuffer.getLastToken();
            const auto& location = tokenBuffer.getSourceLocation(lastToken);
            diagnostics.addUnterminatedStringError(sourceText, location);
        }
    };

    [[nodiscard]] TokenBuffer lex(const SourceTextSharedPtr& sourceText, DiagnosticsBag& diagnostics, u16 fileId) noexcept
    {
        CARACAL_ZONE_NAMED("lex");

        const auto source = std::string_view(sourceText->text);
        const char* currentIndex = source.data();

        // skip optional BOM at start of file
        if (source.length() >= 3 &&
            static_cast<u8>(source[0]) == 0xEF &&
            static_cast<u8>(source[1]) == 0xBB &&
            static_cast<u8>(source[2]) == 0xBF)
        {
            currentIndex += 3;
        }

        // first token's trivia starts where lexing starts
        const auto firstTokenStart = static_cast<i32>(currentIndex - source.data());
        TokenBuffer tokenBuffer{ sourceText, fileId, firstTokenStart };

        while (true)
        {
            const auto current = *currentIndex;
            switch (current)
            {
                case '\r':
                {
                    if (currentIndex[1] == '\n')
                        currentIndex++;

                    currentIndex++;
                    break;
                }
                case '\n':
                {
                    currentIndex++;
                    break;
                }
                case '\t':
                case ' ':
                {
                    currentIndex++;
                    break;
                }
                case '\0':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::EndOfFile, 0);
                    return tokenBuffer;
                }
                case '+':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::Plus, 1);
                    break;
                }
                case '-':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::Minus, 1);
                    break;
                }
                case '*':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::Star, 1);
                    break;
                }
                case '%':
                {
                    const auto nextChar = currentIndex[1];
                    if (nextChar == '+')
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::PercentPlus, 2);
                        break;
                    }
                    if (nextChar == '-')
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::PercentMinus, 2);
                        break;
                    }
                    if (nextChar == '*')
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::PercentStar, 2);
                        break;
                    }

                    // % is not a valid operator
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::Unknown, 1);
                    const auto& lastToken = tokenBuffer.getLastToken();
                    const auto& location = tokenBuffer.getSourceLocation(lastToken);
                    diagnostics.addIllegalCharacterError(sourceText, location);
                    break;
                }
                case '/':
                {
                    const auto nextChar = currentIndex[1];
                    if (nextChar == '/')
                    {
                        // Consume comment until end of line
                        currentIndex += 2;
                        while (*currentIndex != '\n' && *currentIndex != '\0')
                        {
                            currentIndex++;
                        }
                        break;
                    }
                    else if (nextChar == '*')
                    {
                        // Consume multi-line comment until closing */
                        currentIndex += 2;
                        while (true)
                        {
                            if (*currentIndex == '\0')
                            {
                                break;
                            }
                            // check for the end of block comment
                            if (*currentIndex == '*' && currentIndex[1] == '/')
                            {
                                currentIndex += 2;
                                break;
                            }
                            currentIndex++;
                        }
                        break;
                    }
                    else
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::Slash, 1);
                        break;
                    }
                }
                case '.':
                {
                    if (currentIndex[1] == '.' && currentIndex[2] == '.')
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::Ellipsis, 3);
                    }
                    else
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::Dot, 1);
                    }
                    break;
                }
                case ':':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::Colon, 1);
                    break;
                }
                case ';':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::Semicolon, 1);
                    break;
                }
                case '\'':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::SingleQuote, 1);
                    break;
                }
                case '#':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::Hash, 1);
                    break;
                }
                case ',':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::Comma, 1);
                    break;
                }
                case '=':
                {
                    if (currentIndex[1] == '=')
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::EqualEqual, 2);
                    }
                    else
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::Equal, 1);
                    }
                    break;
                }
                case '!':
                {
                    if (currentIndex[1] == '=')
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::BangEqual, 2);
                    }
                    else
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::Bang, 1);
                    }
                    break;
                }
                case '<':
                {
                    if (currentIndex[1] == '=')
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::LessThanEqual, 2);
                    }
                    else
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::LessThan, 1);
                    }
                    break;
                }
                case '>':
                {
                    if (currentIndex[1] == '=')
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::GreaterThanEqual, 2);
                    }
                    else
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::GreaterThan, 1);
                    }
                    break;
                }
                case '(':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::OpenParenthesis, 1);
                    break;
                }
                case ')':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::CloseParenthesis, 1);
                    break;
                }
                case '{':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::OpenBrace, 1);
                    break;
                }
                case '}':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::CloseBrace, 1);
                    break;
                }
                case '[':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::OpenBracket, 1);
                    break;
                }
                case ']':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::CloseBracket, 1);
                    break;
                }
                case '\"':
                {
                    LexString(tokenBuffer, diagnostics, sourceText, source, currentIndex);
                    break;
                }
                default:
                {
                    if (current == '_' && !IsUnderscoreOrLetterOrNumber(currentIndex[1]))
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::Underscore, 1);
                        break;
                    }
                    else if (IsUnderscoreOrLetter(current))
                    {
                        LexIdentifier(tokenBuffer, source, currentIndex);
                        break;
                    }
                    else if (IsNumber(current))
                    {
                        LexNumber(tokenBuffer, source, currentIndex);
                        break;
                    }

                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, TokenKind::Unknown, 1);
                    const auto& lastToken = tokenBuffer.getLastToken();
                    const auto& location = tokenBuffer.getSourceLocation(lastToken);
                    diagnostics.addIllegalCharacterError(sourceText, location);
                    break;
                }
            }
        }
    }
}
