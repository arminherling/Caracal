#include <CaraTest.h>
#include "SourceLocationTests.h"

int main(int argc, char* argv[])
{
    CaraTest::TestRunner runner{ argc, argv };
    auto testSuite = SourceLocationTests();

    return runner.run(testSuite);
}
