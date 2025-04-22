#include <AalTest.h>
#include <iostream>
#include <Syntax/Lexer.h>
#include <Syntax/Token.h>
#include <Syntax/TokenBuffer.h>
#include <Syntax/TokenKind.h>
#include <Text/SourceLocation.h>
#include <Text/SourceText.h>

using namespace AalTest;

namespace 
{
    void SingleSourceLocation(const QString& testName, const SourceTextSharedPtr& input, const SourceLocation& expectedLocation)
    {
        DiagnosticsBag diagnostics;

        auto startTime = std::chrono::high_resolution_clock::now();
        auto tokens = Lex(input, diagnostics);
        auto& token = tokens[0];

        auto endTime = std::chrono::high_resolution_clock::now();
        std::cout << "      Lex(): " << Stringify(endTime - startTime).toStdString() << std::endl;

        auto& location = tokens.getSourceLocation(token);
        AalTest::AreEqual(location.startIndex, expectedLocation.startIndex);
        AalTest::AreEqual(location.endIndex, expectedLocation.endIndex);
        AalTest::AreEqual(location.startColumn, expectedLocation.startColumn);
        AalTest::AreEqual(location.endColumn, expectedLocation.endColumn);
        AalTest::AreEqual(location.startLine, expectedLocation.startLine);
        AalTest::AreEqual(location.endLine, expectedLocation.endLine);
    }

    QList<std::tuple<QString, SourceTextSharedPtr, SourceLocation>> SingleSourceLocation_Data()
    {
        auto source1 = std::make_shared<SourceText>(QString("+"));
        auto source2 = std::make_shared<SourceText>(QString(" bar "));
        auto source3 = std::make_shared<SourceText>(QString("\nreturn"));
        auto source4 = std::make_shared<SourceText>(QString("\r\nreturn"));
        auto source5 = std::make_shared<SourceText>(QString("  1_234 "));
        auto source6 = std::make_shared<SourceText>(QString(" \"1234567890\""));
        auto source7 = std::make_shared<SourceText>(QString("$"));

        return {
            std::make_tuple(
                QString("+"), 
                source1,
                SourceLocation{ .startIndex = 0, .endIndex = 1, .startColumn = 1, .endColumn = 2, .startLine = 1, .endLine = 1}),
            std::make_tuple(
                QString(" bar "),
                source2,
                SourceLocation{ .startIndex = 1, .endIndex = 4, .startColumn = 2, .endColumn = 5, .startLine = 1, .endLine = 1}),
            std::make_tuple(
                QString("\\nreturn"),
                source3,
                SourceLocation{ .startIndex = 1, .endIndex = 7, .startColumn = 1, .endColumn = 7, .startLine = 2, .endLine = 2}),
            std::make_tuple(
                QString("\\r\\nreturn"),
                source4,
                SourceLocation{ .startIndex = 2, .endIndex = 8, .startColumn = 1, .endColumn = 7, .startLine = 2, .endLine = 2}),
            std::make_tuple(
                QString("  1_234 "),
                source5,
                SourceLocation{ .startIndex = 2, .endIndex = 7, .startColumn = 3, .endColumn = 8, .startLine = 1, .endLine = 1}),
            std::make_tuple(
                QString(" \"1234567890\""),
                source6,
                SourceLocation{ .startIndex = 1, .endIndex = 13, .startColumn = 2, .endColumn = 14, .startLine = 1, .endLine = 1}),
            std::make_tuple(
                QString("$"),
                source7,
                SourceLocation{ .startIndex = 0, .endIndex = 1, .startColumn = 1, .endColumn = 2, .startLine = 1, .endLine = 1})
        };
    }

    void MultipleSourceLocations()
    {
        auto input = std::make_shared<SourceText>(QString("define sum(a int, b int) \r\n {\r\n return a + b \r\n}\r\n"));
        auto expectedList = QList<SourceLocation>
        {
            { .startIndex = 0, .endIndex = 6, .startColumn = 1, .endColumn = 7, .startLine = 1, .endLine = 1 },  // define
            { .startIndex = 7, .endIndex = 10, .startColumn = 8, .endColumn = 11, .startLine = 1, .endLine = 1 }, // sum
            { .startIndex = 10, .endIndex = 11, .startColumn = 11, .endColumn = 12, .startLine = 1, .endLine = 1 }, // (
            { .startIndex = 11, .endIndex = 12, .startColumn = 12, .endColumn = 13, .startLine = 1, .endLine = 1 }, // a
            { .startIndex = 13, .endIndex = 16, .startColumn = 14, .endColumn = 17, .startLine = 1, .endLine = 1 }, // int
            { .startIndex = 16, .endIndex = 17, .startColumn = 17, .endColumn = 18, .startLine = 1, .endLine = 1 }, // ,
            { .startIndex = 18, .endIndex = 19, .startColumn = 19, .endColumn = 20, .startLine = 1, .endLine = 1 }, // b
            { .startIndex = 20, .endIndex = 23, .startColumn = 21, .endColumn = 24, .startLine = 1, .endLine = 1 }, // int
            { .startIndex = 23, .endIndex = 24, .startColumn = 24, .endColumn = 25, .startLine = 1, .endLine = 1 }, // )
            { .startIndex = 28, .endIndex = 29, .startColumn = 2, .endColumn = 3, .startLine = 2, .endLine = 2 },  // {
            { .startIndex = 32, .endIndex = 38, .startColumn = 2, .endColumn = 8, .startLine = 3, .endLine = 3 },  // return
            { .startIndex = 39, .endIndex = 40, .startColumn = 9, .endColumn = 10, .startLine = 3, .endLine = 3 },  // a
            { .startIndex = 41, .endIndex = 42, .startColumn = 11, .endColumn = 12, .startLine = 3, .endLine = 3 }, // +
            { .startIndex = 43, .endIndex = 44, .startColumn = 13, .endColumn = 14, .startLine = 3, .endLine = 3 }, // b
            { .startIndex = 47, .endIndex = 48, .startColumn = 1, .endColumn = 2, .startLine = 4, .endLine = 4 },  // }
            { .startIndex = 50, .endIndex = 50, .startColumn = 1, .endColumn = 1, .startLine = 5, .endLine = 5 }, // EOL
        };
        DiagnosticsBag diagnostics;

        auto startTime = std::chrono::high_resolution_clock::now();
        auto tokens = Lex(input, diagnostics);

        auto endTime = std::chrono::high_resolution_clock::now();
        std::cout << "      Lex(): " << Stringify(endTime - startTime).toStdString() << std::endl;

        AalTest::AreEqual(tokens.size(), expectedList.size());
        for (auto i = 0; i < tokens.size(); i++)
        {
            auto& location = tokens.getSourceLocation(tokens[i]);

            AalTest::AreEqual(location.startIndex, expectedList[i].startIndex);
            AalTest::AreEqual(location.endIndex, expectedList[i].endIndex);
            AalTest::AreEqual(location.startColumn, expectedList[i].startColumn);
            AalTest::AreEqual(location.endColumn, expectedList[i].endColumn);
            AalTest::AreEqual(location.startLine, expectedList[i].startLine);
            AalTest::AreEqual(location.endLine, expectedList[i].endLine);
        }
    }
}

AalTest::TestSuite SourceLocationTests()
{
    AalTest::TestSuite suite{};

    suite.add(QString("SingleSourceLocation"), SingleSourceLocation, SingleSourceLocation_Data);
    suite.add(QString("MultipleSourceLocations"), MultipleSourceLocations);

    return suite;
}
