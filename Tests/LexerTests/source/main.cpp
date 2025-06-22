#include <CaraTest.h>
#include "LexerTests.h"

int main(int argc, char* argv[])
{
    CaraTest::TestRunner runner{ argc, argv };
    auto testSuites = LexerTestsSuite();

    return runner.run(testSuites);
}
