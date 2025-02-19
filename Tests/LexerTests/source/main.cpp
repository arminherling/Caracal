#include <AalTest.h>
#include "LexerTests.h"

int main(int argc, char* argv[])
{
    AalTest::TestRunner runner{ argc, argv };
    auto testSuites = LexerTestsSuite();

    runner.run(testSuites);

    return 0;
}
