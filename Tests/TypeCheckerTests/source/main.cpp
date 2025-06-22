#include <CaraTest.h>
#include "TypeCheckerTests.h"

int main(int argc, char* argv[])
{
    CaraTest::TestRunner runner{ argc, argv };
    auto testSuites = TypeCheckerTestsSuite();

    return runner.run(testSuites);
}
