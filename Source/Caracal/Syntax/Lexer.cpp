#include <Caracal/Syntax/Lexer.h>

#include <unordered_map>

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
            { std::string_view("cpp"),      TokenKind::CppKeyword },
        };
    }

    [[nodiscard]] static auto TokenSize(TokenKind kind) noexcept
    {
        static const auto tokenSizes = InitializeTokenSizes();
        if (const auto result = tokenSizes.find(kind); result != tokenSizes.end())
            return result->second;

        return 1;
    }

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

    [[nodiscard]] static auto PeekCurrentChar(std::string_view source, i32 charIndex) noexcept
    {
        if (charIndex >= source.length())
            return '\0';

        return source[charIndex];
    }

    [[nodiscard]] static auto PeekNextChar(std::string_view source, i32& currentIndex) noexcept { return PeekCurrentChar(source, currentIndex + 1); }

    [[nodiscard]] static auto CaptureTrivia(TokenBuffer& tokenBuffer, std::string_view source, i32 currentIndex, i32 triviaStartIndex) noexcept
    {
        const auto triviaLength = currentIndex - triviaStartIndex;
        const auto trivia = source.substr(triviaStartIndex, triviaLength);

        return tokenBuffer.addTrivia(trivia);
    }

    static void AddTokenKindAndAdvance(TokenBuffer& tokenBuffer, std::string_view source, i32& currentIndex, i32& triviaStartIndex, TokenKind tokenKind) noexcept
    {
        const auto tokenSize = TokenSize(tokenKind);
        const auto locationIndex = tokenBuffer.addSourceLocation(
            {
                .startIndex = currentIndex,
                .endIndex = currentIndex + tokenSize
            });
        const auto triviaIndex = CaptureTrivia(tokenBuffer, source, currentIndex, triviaStartIndex);

        currentIndex++;
        triviaStartIndex = currentIndex;

        tokenBuffer.addToken({ .kind = tokenKind, .locationIndex = locationIndex, .triviaIndex = triviaIndex });
    };

    [[nodiscard]] static void AddKindAndLexeme(TokenBuffer& tokenBuffer, std::string_view source, i32 currentIndex, i32& triviaStartIndex, TokenKind tokenKind, i32 startIndex) noexcept
    {
        const auto length = currentIndex - startIndex;
        const auto identifierIndex = tokenBuffer.addLexeme(source.substr(startIndex, length));
        const auto locationIndex = tokenBuffer.addSourceLocation(
            {
                .startIndex = startIndex,
                .endIndex = currentIndex
            });
        const auto triviaIndex = CaptureTrivia(tokenBuffer, source, startIndex, triviaStartIndex);
        triviaStartIndex = currentIndex;

        tokenBuffer.addToken({ .kind = tokenKind, .lexemeIndex = identifierIndex, .locationIndex = locationIndex, .triviaIndex = triviaIndex });
    };

    static auto IdentifierKind(std::string_view source, i32 currentIndex, i32 startIndex) noexcept
    {
        static const auto keywords = InitializeKeywords();
        const auto length = currentIndex - startIndex;
        const auto lexeme = source.substr(startIndex, length);

        if (const auto result = keywords.find(lexeme); result != keywords.end())
            return result->second;

        return TokenKind::Identifier;
    }

    static void LexIdentifier(TokenBuffer& tokenBuffer, std::string_view source, i32& currentIndex, i32& triviaStartIndex) noexcept
    {
        const auto startIndex = currentIndex;
        while (IsUnderscoreOrLetterOrNumber(PeekCurrentChar(source, currentIndex)))
            currentIndex++;

        const auto maybeKeywordKind = IdentifierKind(source, currentIndex, startIndex);

        AddKindAndLexeme(tokenBuffer, source, currentIndex, triviaStartIndex, maybeKeywordKind, startIndex);
    };

    static void LexNumber(TokenBuffer& tokenBuffer, std::string_view source, i32& currentIndex, i32& triviaStartIndex) noexcept
    {
        const auto startIndex = currentIndex;

        auto current = PeekCurrentChar(source, currentIndex);
        while (IsNumber(current) || (current == '_' && PeekNextChar(source, currentIndex) != '.'))
        {
            currentIndex++;
            current = PeekCurrentChar(source, currentIndex);
        }

        if (current == '.' && IsNumber(PeekNextChar(source, currentIndex)))
        {
            currentIndex++;

            while (IsUnderscoreOrNumber(PeekCurrentChar(source, currentIndex)))
                currentIndex++;
        }

        AddKindAndLexeme(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Number, startIndex);
    };

    static void LexString(TokenBuffer& tokenBuffer, DiagnosticsBag& diagnostics, std::string_view source, i32& currentIndex, i32& triviaStartIndex) noexcept
    {
        const auto startIndex = currentIndex;

        // Consume opening quotation mark
        currentIndex++;
        auto current = PeekCurrentChar(source, currentIndex);
        while (current != '\"' && current != '\0')
        {
            if (current == '\\')
            {
                currentIndex++;
            }
            currentIndex++;
            current = PeekCurrentChar(source, currentIndex);
        }

        if (current == '\"')
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
            diagnostics.AddError(DiagnosticKind::_0002_UnterminatedString, location);
        }
    };

    [[nodiscard]] TokenBuffer lex(const SourceTextSharedPtr& sourceText, DiagnosticsBag& diagnostics) noexcept
    {
        TokenBuffer tokenBuffer{ sourceText };
        const auto source = std::string_view(sourceText->text);
        i32 currentIndex = 0;
        i32 triviaStartIndex = 0;

        // skip optional BOM at start of file
        if (source.length() >= 3 &&
            static_cast<u8>(source[0]) == 0xEF &&
            static_cast<u8>(source[1]) == 0xBB &&
            static_cast<u8>(source[2]) == 0xBF)
        {
            currentIndex = 3;
            triviaStartIndex = 3;
        }

        while (true)
        {
            const auto current = PeekCurrentChar(source, currentIndex);
            switch (current)
            {
                case '\r':
                {
                    if (PeekNextChar(source, currentIndex) == '\n')
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
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::EndOfFile);
                    return tokenBuffer;
                }
                case '+':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Plus);
                    break;
                }
                case '-':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Minus);
                    break;
                }
                case '*':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Star);
                    break;
                }
                case '/':
                {
                    const auto nextChar = PeekNextChar(source, currentIndex);
                    if (nextChar == '/')
                    {
                        // Consume comment until end of line
                        currentIndex += 2;
                        auto currentChar = PeekCurrentChar(source, currentIndex);
                        while (currentChar != '\n' && currentChar != '\0')
                        {
                            currentIndex++;
                            currentChar = PeekCurrentChar(source, currentIndex);
                        }
                        break;
                    }
                    else if (nextChar == '*')
                    {
                        // Consume multi-line comment until closing */
                        currentIndex += 2;
                        while (true)
                        {
                            const auto currentChar = PeekCurrentChar(source, currentIndex);
                            if (currentChar == '\0')
                            {
                                break;
                            }
                            // check for the end of block comment
                            if (currentChar == '*' && PeekNextChar(source, currentIndex) == '/')
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
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Slash);
                        break;
                    }
                }
                case '.':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Dot);
                    break;
                }
                case ':':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Colon);
                    break;
                }
                case ';':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Semicolon);
                    break;
                }
                case '\'':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Uptick);
                    break;
                }
                case ',':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Comma);
                    break;
                }
                case '=':
                {
                    if (PeekNextChar(source, currentIndex) == '=')
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::EqualEqual);
                        currentIndex++; // advance for the second '='
                    }
                    else
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Equal);
                    }
                    break;
                }
                case '!':
                {
                    if (PeekNextChar(source, currentIndex) == '=')
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::BangEqual);
                        currentIndex++; // advance for the '='
                    }
                    else
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Bang);
                    }
                    break;
                }
                case '<':
                {
                    if (PeekNextChar(source, currentIndex) == '=')
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::LessThanEqual);
                        currentIndex++; // advance for the '='
                    }
                    else
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::LessThan);
                    }
                    break;
                }
                case '>':
                {
                    if (PeekNextChar(source, currentIndex) == '=')
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::GreaterThanEqual);
                        currentIndex++; // advance for the '='
                    }
                    else
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::GreaterThan);
                    }
                    break;
                }
                case '(':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::OpenParenthesis);
                    break;
                }
                case ')':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::CloseParenthesis);
                    break;
                }
                case '{':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::OpenBracket);
                    break;
                }
                case '}':
                {
                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::CloseBracket);
                    break;
                }
                case '\"':
                {
                    LexString(tokenBuffer, diagnostics, source, currentIndex, triviaStartIndex);
                    break;
                }
                default:
                {
                    if (current == '_' && !IsUnderscoreOrLetterOrNumber(PeekNextChar(source, currentIndex)))
                    {
                        AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Underscore);
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

                    AddTokenKindAndAdvance(tokenBuffer, source, currentIndex, triviaStartIndex, TokenKind::Unknown);
                    const auto& lastToken = tokenBuffer.getLastToken();
                    const auto& location = tokenBuffer.getSourceLocation(lastToken);
                    diagnostics.AddError(DiagnosticKind::_0001_FoundIllegalCharacter, location);
                    break;
                }
            }
        }
    }
}
