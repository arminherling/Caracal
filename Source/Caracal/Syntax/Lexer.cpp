#include <Caracal/Syntax/Lexer.h>
#include <Caracal/Profiling.h>

#include <algorithm>
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

    [[nodiscard]] static auto CaptureTrivia(TokenBuffer& tokenBuffer, std::string_view source, i32 currentIndex, i32 triviaStartIndex) noexcept
    {
        const auto sourceLength = static_cast<i32>(source.length());
        const auto start = std::clamp(triviaStartIndex, 0, sourceLength);
        const auto end = std::clamp(currentIndex, start, sourceLength);
        const auto trivia = source.substr(start, end - start);

        return tokenBuffer.addTrivia(trivia);
    }

    static void AddTokenKindAndAdvance(TokenBuffer& tokenBuffer, std::string_view source, const char*& currentIndex, i32& triviaStartIndex, TokenKind tokenKind, i32 tokenSize) noexcept
    {
        const auto startIndex = static_cast<i32>(currentIndex - source.data());
        const auto locationIndex = tokenBuffer.addSourceLocation(
            {
                .startIndex = startIndex,
                .endIndex = startIndex + tokenSize
            });
        const auto triviaIndex = CaptureTrivia(tokenBuffer, source, startIndex, triviaStartIndex);

        currentIndex += tokenSize;
        triviaStartIndex = startIndex + tokenSize;

        tokenBuffer.addToken({ .kind = tokenKind, .locationIndex = locationIndex, .triviaIndex = triviaIndex });
    };

    static void AddKindAndLexeme(TokenBuffer& tokenBuffer, std::string_view source, const char* currentIndex, i32& triviaStartIndex, TokenKind tokenKind, i32 startIndex) noexcept
    {
        const auto endIndex = static_cast<i32>(currentIndex - source.data());
        const auto length = endIndex - startIndex;
        const auto identifierIndex = tokenBuffer.addLexeme(source.substr(startIndex, length));
        const auto locationIndex = tokenBuffer.addSourceLocation(
            {
                .startIndex = startIndex,
                .endIndex = endIndex
            });
        const auto triviaIndex = CaptureTrivia(tokenBuffer, source, startIndex, triviaStartIndex);
        triviaStartIndex = endIndex;

        tokenBuffer.addToken({ .kind = tokenKind, .lexemeIndex = identifierIndex, .locationIndex = locationIndex, .triviaIndex = triviaIndex });
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

    static void LexIdentifier(TokenBuffer& tokenBuffer, std::string_view source, const char*& currentIndex, i32& triviaStartIndex) noexcept
    {
        const char* start = currentIndex;
        while (IsUnderscoreOrLetterOrNumber(*currentIndex))
            currentIndex++;

        const auto startIndex = static_cast<i32>(start - source.data());
        const auto maybeKeywordKind = IdentifierKind(start, static_cast<i32>(currentIndex - start));

        AddKindAndLexeme(tokenBuffer, source, currentIndex, triviaStartIndex, maybeKeywordKind, startIndex);
    };

    static void LexNumber(TokenBuffer& tokenBuffer, std::string_view source, const char*& currentIndex, i32& triviaStartIndex) noexcept
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

        AddKindAndLexeme(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Number, startIndex);
    };

    static void LexString(
        TokenBuffer& tokenBuffer,
        DiagnosticsBag& diagnostics,
        const SourceTextSharedPtr& sourceText,
        std::string_view source,
        const char*& currentIndex,
        i32& triviaStartIndex) noexcept
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
            AddKindAndLexeme(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::String, startIndex);
        }
        else
        {
            AddKindAndLexeme(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Error, startIndex);
            const auto& lastToken = tokenBuffer.getLastToken();
            const auto& location = tokenBuffer.getSourceLocation(lastToken);
            diagnostics.addUnterminatedStringError(sourceText, location);
        }
    };

    [[nodiscard]] TokenBuffer lex(const SourceTextSharedPtr& sourceText, DiagnosticsBag& diagnostics) noexcept
    {
        CARACAL_ZONE_NAMED("lex");

        TokenBuffer tokenBuffer{ sourceText };
        const auto source = std::string_view(sourceText->text);
        const char* currentIndex = source.data();
        i32 triviaStartIndex = 0;

        // skip optional BOM at start of file
        if (source.length() >= 3 &&
            static_cast<u8>(source[0]) == 0xEF &&
            static_cast<u8>(source[1]) == 0xBB &&
            static_cast<u8>(source[2]) == 0xBF)
        {
            currentIndex += 3;
            triviaStartIndex = 3;
        }

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
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::EndOfFile, 0);
                    return tokenBuffer;
                }
                case '+':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Plus, 1);
                    break;
                }
                case '-':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Minus, 1);
                    break;
                }
                case '*':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Star, 1);
                    break;
                }
                case '%':
                {
                    const auto nextChar = currentIndex[1];
                    if (nextChar == '+')
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::PercentPlus, 2);
                        break;
                    }
                    if (nextChar == '-')
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::PercentMinus, 2);
                        break;
                    }
                    if (nextChar == '*')
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::PercentStar, 2);
                        break;
                    }

                    // % is not a valid operator
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Unknown, 1);
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
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Slash, 1);
                        break;
                    }
                }
                case '.':
                {
                    if (currentIndex[1] == '.' && currentIndex[2] == '.')
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Ellipsis, 3);
                    }
                    else
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Dot, 1);
                    }
                    break;
                }
                case ':':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Colon, 1);
                    break;
                }
                case ';':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Semicolon, 1);
                    break;
                }
                case '\'':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::SingleQuote, 1);
                    break;
                }
                case '#':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Hash, 1);
                    break;
                }
                case ',':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Comma, 1);
                    break;
                }
                case '=':
                {
                    if (currentIndex[1] == '=')
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::EqualEqual, 2);
                    }
                    else
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Equal, 1);
                    }
                    break;
                }
                case '!':
                {
                    if (currentIndex[1] == '=')
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::BangEqual, 2);
                    }
                    else
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Bang, 1);
                    }
                    break;
                }
                case '<':
                {
                    if (currentIndex[1] == '=')
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::LessThanEqual, 2);
                    }
                    else
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::LessThan, 1);
                    }
                    break;
                }
                case '>':
                {
                    if (currentIndex[1] == '=')
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::GreaterThanEqual, 2);
                    }
                    else
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::GreaterThan, 1);
                    }
                    break;
                }
                case '(':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::OpenParenthesis, 1);
                    break;
                }
                case ')':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::CloseParenthesis, 1);
                    break;
                }
                case '{':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::OpenBrace, 1);
                    break;
                }
                case '}':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::CloseBrace, 1);
                    break;
                }
                case '[':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::OpenBracket, 1);
                    break;
                }
                case ']':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::CloseBracket, 1);
                    break;
                }
                case '\"':
                {
                    LexString(tokenBuffer, diagnostics, sourceText, source, currentIndex, triviaStartIndex);
                    break;
                }
                default:
                {
                    if (current == '_' && !IsUnderscoreOrLetterOrNumber(currentIndex[1]))
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Underscore, 1);
                        break;
                    }
                    else if (IsUnderscoreOrLetter(current))
                    {
                        LexIdentifier(tokenBuffer, source, currentIndex, triviaStartIndex);
                        break;
                    }
                    else if (IsNumber(current))
                    {
                        LexNumber(tokenBuffer, source, currentIndex, triviaStartIndex);
                        break;
                    }

                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Unknown, 1);
                    const auto& lastToken = tokenBuffer.getLastToken();
                    const auto& location = tokenBuffer.getSourceLocation(lastToken);
                    diagnostics.addIllegalCharacterError(sourceText, location);
                    break;
                }
            }
        }
    }
}
