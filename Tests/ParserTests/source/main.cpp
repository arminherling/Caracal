#include <CaraTest.h>
#include "ParserTests.h"

int main(int argc, char* argv[])
{
    CaraTest::TestRunner runner{ argc, argv };
    auto testSuites = ParserTestsSuite();

    return runner.run(testSuites);
}
