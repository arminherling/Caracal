#include <CaraTest.h>
#include <iostream>
#include <Caracal/Compilation.h>
#include <Caracal/Syntax/Token.h>
#include <Caracal/Syntax/TokenBuffer.h>
#include <Caracal/Syntax/TokenKind.h>
#include <Caracal/Text/File.h>
#include <Caracal/Text/SourceLocation.h>
#include <Caracal/Text/SourceText.h>

static void SingleSourceLocation(
    const std::string& /*testName*/, 
    const Caracal::SourceTextSharedPtr& input, 
    const Caracal::SourceLocation& expectedLocation)
{
    Caracal::DiagnosticsBag diagnostics;

    auto startTime = std::chrono::high_resolution_clock::now();
    auto tokens = Caracal::lex(input, diagnostics);
    auto token = tokens.getToken(0);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      lex(): " << CaraTest::stringify(endTime - startTime) << std::endl;

    auto& location = tokens.getSourceLocation(token);
    CaraTest::areEqual(expectedLocation.startIndex, location.startIndex);
    CaraTest::areEqual(expectedLocation.endIndex, location.endIndex);
}

static std::vector<std::tuple<std::string, Caracal::SourceTextSharedPtr, Caracal::SourceLocation>> SingleSourceLocation_Data()
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
    auto expectedList = std::vector<Caracal::SourceLocation>
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

    CaraTest::areEqual(expectedList.size(), tokens.size());
    for (auto i = 0; i < tokens.size(); i++)
    {
        auto& location = tokens.getSourceLocation(tokens.getToken(i));

        CaraTest::areEqual(expectedList[i].startIndex, location.startIndex);
        CaraTest::areEqual(expectedList[i].endIndex, location.endIndex);
    }
}

static void CheckLineColumn(Caracal::SourceText& source, i32 offset, i32 expectedLine, i32 expectedColumn)
{
    const auto lineColumn = source.lineColumnAt(offset);
    CaraTest::areEqual(expectedLine, lineColumn.line);
    CaraTest::areEqual(expectedColumn, lineColumn.column);
}

static void LineStartsAndColumns()
{
    // empty file: one line, offset 0 is line 1 column 1
    Caracal::SourceText empty{ "" };
    CaraTest::areEqual(size_t{ 1 }, empty.lineStarts().size());
    CheckLineColumn(empty, 0, 1, 1);

    // LF only, final line without a trailing newline
    Caracal::SourceText plain{ "ab\ncd" };
    CaraTest::areEqual(size_t{ 2 }, plain.lineStarts().size());
    CheckLineColumn(plain, 0, 1, 1);
    CheckLineColumn(plain, 2, 1, 3);  // the newline itself belongs to line 1
    CheckLineColumn(plain, 3, 2, 1);
    CheckLineColumn(plain, 5, 2, 3);  // end-of-file offset is one past the last character

    // CRLF: the line starts after the LF, the CR belongs to the previous line
    Caracal::SourceText crlf{ "a\r\nbb\r\n" };
    CaraTest::areEqual(size_t{ 3 }, crlf.lineStarts().size());
    CheckLineColumn(crlf, 1, 1, 2);
    CheckLineColumn(crlf, 3, 2, 1);
    CheckLineColumn(crlf, 4, 2, 2);
    CheckLineColumn(crlf, 7, 3, 1);

    // BOM bytes count as columns of line 1
    Caracal::SourceText bom{ "\xEF\xBB\xBF" "abc\ndef" };
    CaraTest::areEqual(size_t{ 2 }, bom.lineStarts().size());
    CheckLineColumn(bom, 3, 1, 4);
    CheckLineColumn(bom, 7, 2, 1);
}

static void LineStartsMatchNaiveCount()
{
    const auto currentFilePath = std::filesystem::path(__FILE__);
    const auto inputDirectory = std::filesystem::absolute(currentFilePath.parent_path() / "../../TestData/Input");

    for (const auto& entry : std::filesystem::recursive_directory_iterator(inputDirectory))
    {
        if (!entry.is_regular_file() || entry.path().extension() != ".cara")
        {
            continue;
        }

        const auto data = Caracal::File::readText(entry.path());
        if (!data.has_value())
        {
            CaraTest::fail();
        }

        Caracal::SourceText source{ data.value() };

        std::vector<i32> naiveStarts{ 0 };
        for (size_t index = 0; index < data.value().size(); index++)
        {
            if (data.value()[index] == '\n')
            {
                naiveStarts.push_back(static_cast<i32>(index) + 1);
            }
        }

        CaraTest::isTrue(source.lineStarts() == naiveStarts);
    }
}

static auto tests =
{
    CaraTest::addTest("SingleSourceLocation", SingleSourceLocation, SingleSourceLocation_Data),
    CaraTest::addTest("MultipleSourceLocations", MultipleSourceLocations),
    CaraTest::addTest("LineStartsAndColumns", LineStartsAndColumns),
    CaraTest::addTest("LineStartsMatchNaiveCount", LineStartsMatchNaiveCount),
};
