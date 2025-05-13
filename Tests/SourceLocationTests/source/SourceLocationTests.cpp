#include <AalTest.h>
#include <iostream>
#include <Syntax/Lexer.h>
#include <Syntax/Token.h>
#include <Syntax/TokenBuffer.h>
#include <Syntax/TokenKind.h>
#include <Text/SourceLocation.h>
#include <Text/SourceText.h>

namespace 
{
    void SingleSourceLocation(const QString& testName, const Caracal::SourceTextSharedPtr& input, const Caracal::SourceLocation& expectedLocation)
    {
        Caracal::DiagnosticsBag diagnostics;

        auto startTime = std::chrono::high_resolution_clock::now();
        auto tokens = Caracal::lex(input, diagnostics);
        auto& token = tokens.getToken(0);

        auto endTime = std::chrono::high_resolution_clock::now();
        std::cout << "      lex(): " << AalTest::Stringify(endTime - startTime).toStdString() << std::endl;

        auto& location = tokens.getSourceLocation(token);
        AalTest::AreEqual(location.startIndex, expectedLocation.startIndex);
        AalTest::AreEqual(location.endIndex, expectedLocation.endIndex);
    }

    QList<std::tuple<QString, Caracal::SourceTextSharedPtr, Caracal::SourceLocation>> SingleSourceLocation_Data()
    {
        auto source1 = std::make_shared<Caracal::SourceText>(QString("+"));
        auto source2 = std::make_shared<Caracal::SourceText>(QString(" bar "));
        auto source3 = std::make_shared<Caracal::SourceText>(QString("\nreturn"));
        auto source4 = std::make_shared<Caracal::SourceText>(QString("\r\nreturn"));
        auto source5 = std::make_shared<Caracal::SourceText>(QString("  1_234 "));
        auto source6 = std::make_shared<Caracal::SourceText>(QString(" \"1234567890\""));
        auto source7 = std::make_shared<Caracal::SourceText>(QString("$"));

        return {
            std::make_tuple(
                QString("+"), 
                source1,
                Caracal::SourceLocation{ .startIndex = 0, .endIndex = 1 }),
            std::make_tuple(
                QString(" bar "),
                source2,
                Caracal::SourceLocation{ .startIndex = 1, .endIndex = 4 }),
            std::make_tuple(
                QString("\\nreturn"),
                source3,
                Caracal::SourceLocation{ .startIndex = 1, .endIndex = 7 }),
            std::make_tuple(
                QString("\\r\\nreturn"),
                source4,
                Caracal::SourceLocation{ .startIndex = 2, .endIndex = 8 }),
            std::make_tuple(
                QString("  1_234 "),
                source5,
                Caracal::SourceLocation{ .startIndex = 2, .endIndex = 7 }),
            std::make_tuple(
                QString(" \"1234567890\""),
                source6,
                Caracal::SourceLocation{ .startIndex = 1, .endIndex = 13 }),
            std::make_tuple(
                QString("$"),
                source7,
                Caracal::SourceLocation{ .startIndex = 0, .endIndex = 1 })
        };
    }

    void MultipleSourceLocations()
    {
        auto input = std::make_shared<Caracal::SourceText>(QString("define sum(a int, b int) \r\n {\r\n return a + b \r\n}\r\n"));
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
        std::cout << "      lex(): " << AalTest::Stringify(endTime - startTime).toStdString() << std::endl;

        AalTest::AreEqual(tokens.size(), expectedList.size());
        for (auto i = 0; i < tokens.size(); i++)
        {
            auto& location = tokens.getSourceLocation(tokens.getToken(i));

            AalTest::AreEqual(location.startIndex, expectedList[i].startIndex);
            AalTest::AreEqual(location.endIndex, expectedList[i].endIndex);
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
