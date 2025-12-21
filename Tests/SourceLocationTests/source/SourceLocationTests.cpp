#include <CaraTest.h>
#include <iostream>
#include <Syntax/Lexer.h>
#include <Syntax/Token.h>
#include <Syntax/TokenBuffer.h>
#include <Syntax/TokenKind.h>
#include <Text/SourceLocation.h>
#include <Text/SourceText.h>
#include <QList>

static void SingleSourceLocation(const std::string& /*testName*/, const Caracal::SourceTextSharedPtr& input, const Caracal::SourceLocation& expectedLocation)
{
    Caracal::DiagnosticsBag diagnostics;

    auto startTime = std::chrono::high_resolution_clock::now();
    auto tokens = Caracal::lex(input, diagnostics);
    auto& token = tokens.getToken(0);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      lex(): " << CaraTest::stringify(endTime - startTime) << std::endl;

    auto& location = tokens.getSourceLocation(token);
    CaraTest::areEqual(location.startIndex, expectedLocation.startIndex);
    CaraTest::areEqual(location.endIndex, expectedLocation.endIndex);
}

static QList<std::tuple<std::string, Caracal::SourceTextSharedPtr, Caracal::SourceLocation>> SingleSourceLocation_Data()
{
    auto source1 = std::make_shared<Caracal::SourceText>("+");
    auto source2 = std::make_shared<Caracal::SourceText>(" bar ");
    auto source3 = std::make_shared<Caracal::SourceText>("\nreturn");
    auto source4 = std::make_shared<Caracal::SourceText>("\r\nreturn");
    auto source5 = std::make_shared<Caracal::SourceText>("  1_234 ");
    auto source6 = std::make_shared<Caracal::SourceText>(" \"1234567890\"");
    auto source7 = std::make_shared<Caracal::SourceText>("$");

    return {
        std::make_tuple(
            "+", 
            source1,
            Caracal::SourceLocation{ .startIndex = 0, .endIndex = 1 }),
        std::make_tuple(
            " bar ",
            source2,
            Caracal::SourceLocation{ .startIndex = 1, .endIndex = 4 }),
        std::make_tuple(
            "\\nreturn",
            source3,
            Caracal::SourceLocation{ .startIndex = 1, .endIndex = 7 }),
        std::make_tuple(
            "\\r\\nreturn",
            source4,
            Caracal::SourceLocation{ .startIndex = 2, .endIndex = 8 }),
        std::make_tuple(
            "  1_234 ",
            source5,
            Caracal::SourceLocation{ .startIndex = 2, .endIndex = 7 }),
        std::make_tuple(
            " \"1234567890\"",
            source6,
            Caracal::SourceLocation{ .startIndex = 1, .endIndex = 13 }),
        std::make_tuple(
            "$",
            source7,
            Caracal::SourceLocation{ .startIndex = 0, .endIndex = 1 })
    };
}

static void MultipleSourceLocations()
{
    auto input = std::make_shared<Caracal::SourceText>("define sum(a int, b int) \r\n {\r\n return a + b \r\n}\r\n");
    auto expectedList = QList<Caracal::SourceLocation>
    {
        { .startIndex = 0, .endIndex = 6 },  // define
        { .startIndex = 7, .endIndex = 10 }, // sum
        { .startIndex = 10, .endIndex = 11 }, // (
        { .startIndex = 11, .endIndex = 12 }, // a
        { .startIndex = 13, .endIndex = 16 }, // int
        { .startIndex = 16, .endIndex = 17 }, // ,
        { .startIndex = 18, .endIndex = 19 }, // b
        { .startIndex = 20, .endIndex = 23 }, // int
        { .startIndex = 23, .endIndex = 24 }, // )
        { .startIndex = 28, .endIndex = 29 },  // {
        { .startIndex = 32, .endIndex = 38 },  // return
        { .startIndex = 39, .endIndex = 40 },  // a
        { .startIndex = 41, .endIndex = 42 }, // +
        { .startIndex = 43, .endIndex = 44 }, // b
        { .startIndex = 47, .endIndex = 48 },  // }
        { .startIndex = 50, .endIndex = 50 }, // EOL
    };
    Caracal::DiagnosticsBag diagnostics;

    auto startTime = std::chrono::high_resolution_clock::now();
    auto tokens = Caracal::lex(input, diagnostics);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      lex(): " << CaraTest::stringify(endTime - startTime) << std::endl;

    CaraTest::areEqual(tokens.size(), expectedList.size());
    for (auto i = 0; i < tokens.size(); i++)
    {
        auto& location = tokens.getSourceLocation(tokens.getToken(i));

        CaraTest::areEqual(location.startIndex, expectedList[i].startIndex);
        CaraTest::areEqual(location.endIndex, expectedList[i].endIndex);
    }
}

static auto tests =
{
    CaraTest::addTest("SingleSourceLocation", SingleSourceLocation, SingleSourceLocation_Data),
    CaraTest::addTest("MultipleSourceLocations", MultipleSourceLocations),
};
