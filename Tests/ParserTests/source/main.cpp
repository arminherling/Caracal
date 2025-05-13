#include <AalTest.h>
#include "ParserTests.h"

int main(int argc, char* argv[])
{
    AalTest::TestRunner runner{ argc, argv };
    auto testSuites = ParserTestsSuite();

    runner.run(testSuites);

    return 0;
}
