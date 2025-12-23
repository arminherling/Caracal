#include <CaraTest.h>

#include <Caracal/Syntax/Lexer.h>
#include <Caracal/Syntax/Token.h>
#include <Caracal/Syntax/TokenKind.h>
#include <Caracal/Syntax/TokenBuffer.h>
#include <Caracal/Text/File.h>
#include <Caracal/Text/SourceText.h>
#include <iostream>

static void ExpectedTokenKind(const std::string& /*testName*/, const std::string& input, TokenKind expectedKind)
{
    const auto startTime = std::chrono::high_resolution_clock::now();

    const auto source = std::make_shared<Caracal::SourceText>(input);
    Caracal::DiagnosticsBag diagnostics;

    const auto tokens = Caracal::lex(source, diagnostics);
    const auto& token = tokens.getToken(0);

    const auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      lex(): " << CaraTest::stringify(endTime - startTime) << std::endl;

    CaraTest::areEqual(expectedKind, token.kind);
    // expected token + eof token
    CaraTest::areEqual(tokens.size(), 2);
}

static std::vector<std::tuple<std::string, std::string, TokenKind>> Symbols_Data()
{
    return {
        std::make_tuple("Plus", "+", TokenKind::Plus),
        std::make_tuple("Minus", "-", TokenKind::Minus),
        std::make_tuple("Star", "*", TokenKind::Star),
        std::make_tuple("Slash", "/", TokenKind::Slash),

        std::make_tuple("Dot", ".", TokenKind::Dot),
        std::make_tuple("Comma", ",", TokenKind::Comma),
        std::make_tuple("Colon", ":", TokenKind::Colon),
        std::make_tuple("Semicolon", ";", TokenKind::Semicolon),
        std::make_tuple("Underscore", "_", TokenKind::Underscore),
        std::make_tuple("Uptick", "'", TokenKind::Uptick),
            
        std::make_tuple("Equal", "=", TokenKind::Equal),
        std::make_tuple("EqualEqual", "==", TokenKind::EqualEqual),
        std::make_tuple("Bang", "!", TokenKind::Bang),
        std::make_tuple("BangEqual", "!=", TokenKind::BangEqual),
        std::make_tuple("LessThan", "<", TokenKind::LessThan),
        std::make_tuple("LessThanEqual", "<=", TokenKind::LessThanEqual),
        std::make_tuple("GreaterThan", ">", TokenKind::GreaterThan),
        std::make_tuple("GreaterThanEqual", ">=", TokenKind::GreaterThanEqual),

        std::make_tuple("OpenParenthesis", "(", TokenKind::OpenParenthesis),
        std::make_tuple("CloseParenthesis", ")", TokenKind::CloseParenthesis),
        std::make_tuple("OpenBracket", "{", TokenKind::OpenBracket),
        std::make_tuple("CloseBracket", "}", TokenKind::CloseBracket),

        std::make_tuple("Unknown", "$", TokenKind::Unknown),
    };
}

static std::vector<std::tuple<std::string, std::string, TokenKind>> Keyword_Data()
{
    return {
        std::make_tuple("Def", "def", TokenKind::DefKeyword),
        std::make_tuple("Enum", "enum", TokenKind::EnumKeyword),
        std::make_tuple("Type", "type", TokenKind::TypeKeyword),
        std::make_tuple("If", "if", TokenKind::IfKeyword),
        std::make_tuple("Else", "else", TokenKind::ElseKeyword),
        std::make_tuple("While", "while", TokenKind::WhileKeyword),
        std::make_tuple("Break", "break", TokenKind::BreakKeyword),
        std::make_tuple("Skip", "skip", TokenKind::SkipKeyword),
        std::make_tuple("Return", "return", TokenKind::ReturnKeyword),
        std::make_tuple("True", "true", TokenKind::TrueKeyword),
        std::make_tuple("False", "false", TokenKind::FalseKeyword),
        std::make_tuple("And", "and", TokenKind::AndKeyword),
        std::make_tuple("Or", "or", TokenKind::OrKeyword),
        std::make_tuple("Ref", "ref", TokenKind::RefKeyword),
        std::make_tuple("C++", "cpp", TokenKind::CppKeyword),
    };
}

static void IgnoresWhitespaces(const std::string& input)
{
    const auto startTime = std::chrono::high_resolution_clock::now();

    const auto source = std::make_shared<Caracal::SourceText>(input);
    Caracal::DiagnosticsBag diagnostics;

    const auto tokens = Caracal::lex(source, diagnostics);
    const auto& token = tokens.getToken(0);

    const auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      lex(): " << CaraTest::stringify(endTime - startTime) << std::endl;

    CaraTest::areEqual(TokenKind::EndOfFile, token.kind);
}

static std::vector<std::tuple<std::string>> IgnoresWhitespaces_Data()
{
    return {
        std::make_tuple(""),
        std::make_tuple(" "),
        std::make_tuple("     "),
        std::make_tuple("\t"),
        std::make_tuple("\r"),
        std::make_tuple("\n"),
        std::make_tuple("\r\n"),
        std::make_tuple("\0")
    };
}

static void Identifiers(const std::string& input, const std::string_view& expectedLexeme)
{
    const auto startTime = std::chrono::high_resolution_clock::now();

    const auto source = std::make_shared<Caracal::SourceText>(input);
    Caracal::DiagnosticsBag diagnostics;

    const auto tokens = Caracal::lex(source, diagnostics);
    const auto& token = tokens.getToken(0);

    const auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      lex(): " << CaraTest::stringify(endTime - startTime) << std::endl;

    CaraTest::areEqual(TokenKind::Identifier, token.kind);
    const auto lexeme = tokens.getLexeme(token);
    CaraTest::areEqual(expectedLexeme, lexeme);
}

static std::vector<std::tuple<std::string, std::string_view>> Identifiers_Data()
{
    return {
        std::make_tuple("x", "x"),
        std::make_tuple("foo", "foo"),
        std::make_tuple(" bar ", "bar"),
        std::make_tuple("i32", "i32"),
        std::make_tuple("use", "use"),
        std::make_tuple("class", "class"),
        std::make_tuple("define", "define"),
        std::make_tuple(" _name", "_name"),
        std::make_tuple("m_index", "m_index"),
        std::make_tuple("_10", "_10"),
        std::make_tuple("\n returned", "returned"),
        std::make_tuple("enumeration", "enumeration"),
    };
}

static void Numbers(const std::string& input, const std::string_view& expectedLexeme)
{
    const auto startTime = std::chrono::high_resolution_clock::now();

    const auto source = std::make_shared<Caracal::SourceText>(input);
    Caracal::DiagnosticsBag diagnostics;

    const auto tokens = Caracal::lex(source, diagnostics);
    const auto& token = tokens.getToken(0);

    const auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      lex(): " << CaraTest::stringify(endTime - startTime) << std::endl;

    CaraTest::areEqual(TokenKind::Number, token.kind);
    const auto lexeme = tokens.getLexeme(token);
    CaraTest::areEqual(expectedLexeme, lexeme);
}

static std::vector<std::tuple<std::string, std::string_view>> Numbers_Data()
{
    return {
        std::make_tuple("0", "0"),
        std::make_tuple("  1234 ", "1234"),
        std::make_tuple("  1_234 ", "1_234"),
        std::make_tuple("12.", "12"),
        std::make_tuple("12.34", "12.34"),
        std::make_tuple("1_2.3_4", "1_2.3_4"),
        std::make_tuple("12.34_", "12.34_"),
        std::make_tuple("12.34. ", "12.34"),
        std::make_tuple(" 1234567890", "1234567890")
    };
}

static void Strings(const std::string& input, const std::string_view& expectedLexeme)
{
    const auto startTime = std::chrono::high_resolution_clock::now();

    const auto source = std::make_shared<Caracal::SourceText>(input);
    Caracal::DiagnosticsBag diagnostics;

    const auto tokens = Caracal::lex(source, diagnostics);
    const auto& token = tokens.getToken(0);

    const auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      lex(): " << CaraTest::stringify(endTime - startTime) << std::endl;

    CaraTest::areEqual(TokenKind::String, token.kind);
    const auto lexeme = tokens.getLexeme(token);
    CaraTest::areEqual(expectedLexeme, lexeme);
}

static std::vector<std::tuple<std::string, std::string_view>> Strings_Data()
{
    return {
        std::make_tuple(" \"\" ", "\"\""),
        std::make_tuple("  \"1234\" ", "\"1234\""),
        std::make_tuple("\"string with whitespace\" ", "\"string with whitespace\""),
        std::make_tuple(" \"1234567890\"", "\"1234567890\"")
    };
}

static std::vector<std::tuple<std::string, std::string_view>> StringsWithEscapes_Data()
{
    return {
        std::make_tuple("\"single quote\\'\"", "\"single quote\\'\""),
        std::make_tuple("\"double quote\\\"\"", "\"double quote\\\"\""),
        std::make_tuple("\"backslash\\\\\"", "\"backslash\\\\\""),
        std::make_tuple("\"audible bell\\a\"", "\"audible bell\\a\""),
        std::make_tuple("\"backspace\\b\"", "\"backspace\\b\""),
        std::make_tuple("\"form feed\\f\"", "\"form feed\\f\""),
        std::make_tuple("\"line feed\\n\"", "\"line feed\\n\""),
        std::make_tuple("\"carriage return\\r\"", "\"carriage return\\r\""),
        std::make_tuple("\"horizonal tab\\t\"", "\"horizonal tab\\t\""),
        std::make_tuple("\"vertical tab\\v\"", "\"vertical tab\\v\""),
    };
}

static void UnterminatedStrings(const std::string& input, const std::string_view& expectedLexeme)
{
    const auto startTime = std::chrono::high_resolution_clock::now();

    const auto source = std::make_shared<Caracal::SourceText>(input);
    Caracal::DiagnosticsBag diagnostics;

    const auto tokens = Caracal::lex(source, diagnostics);
    const auto& token = tokens.getToken(0);

    CaraTest::isTrue(!diagnostics.Diagnostics().empty());

    const auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      lex(): " << CaraTest::stringify(endTime - startTime) << std::endl;

    CaraTest::areEqual(TokenKind::Error, token.kind);
    const auto lexeme = tokens.getLexeme(token);
    CaraTest::areEqual(expectedLexeme, lexeme);
}

static std::vector<std::tuple<std::string, std::string_view>> UnterminatedStrings_Data()
{
    return {
        std::make_tuple(" \" ", "\" "),
        std::make_tuple("  \"1234 ", "\"1234 "),
        std::make_tuple("\"string with whitespace ", "\"string with whitespace "),
        std::make_tuple(" \"1234567890", "\"1234567890")
    };
}

static void WhiteSpaceTrivia(const std::string& input, const std::string& expectedTrivia)
{
    const auto startTime = std::chrono::high_resolution_clock::now();

    const auto source = std::make_shared<Caracal::SourceText>(input);
    Caracal::DiagnosticsBag diagnostics;

    const auto tokens = Caracal::lex(source, diagnostics);
    const auto& token = tokens.getToken(0);

    const auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      lex(): " << CaraTest::stringify(endTime - startTime) << std::endl;

    const auto lexeme = tokens.getTrivia(token);
    CaraTest::areEqual(expectedTrivia, lexeme);
}

static std::vector<std::tuple<std::string, std::string>> WhiteSpaceTrivia_Data()
{
    return {
        std::make_tuple(std::string(""), std::string("")),
        std::make_tuple(std::string(" 1234567890"), std::string(" ")),
        std::make_tuple(std::string(" \"hello\""), std::string(" ")),
        std::make_tuple(std::string(" bool"), std::string(" ")),
        std::make_tuple(std::string("   // 1234567890"), std::string("   // 1234567890")),
        std::make_tuple(std::string("// hello\n123"), std::string("// hello\n")),
        std::make_tuple(std::string(" /* block comment */ 123"), std::string(" /* block comment */ ")),
        std::make_tuple(std::string(" 1  id   \"hi\"    "), std::string(" ")),
    };
}

static void WholeInput(const std::string& input, i32 tokenCount)
{
    const auto startTime = std::chrono::high_resolution_clock::now();

    const auto source = std::make_shared<Caracal::SourceText>(input);
    Caracal::DiagnosticsBag diagnostics;

    const auto tokens = Caracal::lex(source, diagnostics);

    const auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      lex(): " << CaraTest::stringify(endTime - startTime) << std::endl;

    CaraTest::isTrue(diagnostics.Diagnostics().empty());
    CaraTest::areEqual(tokenCount, tokens.size());
}

static std::vector<std::tuple<std::string, i32>> WholeInput_Data()
{
    return {
        std::make_tuple("", 1),
        std::make_tuple("name", 2),
        std::make_tuple("use name", 3),
        std::make_tuple("return (x, y)", 7),
        //std::make_tuple("a = () => 3", 8),
        std::make_tuple("enum Value { A B = 5 C D }", 11),
        std::make_tuple("define sum(a int, b int) { return a + b }", 16)
    };
}

static void OneMillionLinesTime()
{
#ifdef _DEBUG
    CaraTest::skip();// ("";
#endif

    auto currentFilePath = std::filesystem::path(__FILE__);
    auto currentDirectory = currentFilePath.parent_path();
    auto testDataPath = currentDirectory / "../../TestData/Input/oneMilLines.txt";
    auto absolutePath = std::filesystem::absolute(testDataPath);
    if (!std::filesystem::exists(absolutePath))
    {
        CaraTest::fail();// ("In file missing");
    }

    const auto data = Caracal::File::readText(absolutePath);
    if (!data.has_value())
    {
        CaraTest::fail();// ("Could not read oneMilLines.txt");
    }

    const auto source = std::make_shared<Caracal::SourceText>(data.value());
    Caracal::DiagnosticsBag diagnostics;

    const auto startTime = std::chrono::high_resolution_clock::now();
    const auto tokens = Caracal::lex(source, diagnostics);

    const auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      lex(): " << CaraTest::stringify(endTime - startTime) << std::endl;
}

static const auto tests =
{
    CaraTest::addTest("SingleCharacter", ExpectedTokenKind, Symbols_Data),
    CaraTest::addTest("IgnoresWhitespaces", IgnoresWhitespaces, IgnoresWhitespaces_Data),
    CaraTest::addTest("Identifiers", Identifiers, Identifiers_Data),
    CaraTest::addTest("Numbers", Numbers, Numbers_Data),
    CaraTest::addTest("Strings", Strings, Strings_Data),
    CaraTest::addTest("StringsWithEscapes", StringsWithEscapes_Data),
    CaraTest::addTest("UnterminatedStrings", UnterminatedStrings, UnterminatedStrings_Data),
    CaraTest::addTest("Keywords", ExpectedTokenKind, Keyword_Data),
    CaraTest::addTest("WhiteSpaceTrivia", WhiteSpaceTrivia, WhiteSpaceTrivia_Data),
    CaraTest::addTest("WholeInput", WholeInput, WholeInput_Data),
    CaraTest::addTest("OneMillionLinesTime", OneMillionLinesTime),
};
