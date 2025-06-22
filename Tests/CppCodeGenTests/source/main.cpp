#include <CaraTest.h>
#include "CppCodeGenTests.h"

int main(int argc, char* argv[])
{
    CaraTest::TestRunner runner{ argc, argv };
    auto testSuites = CppCodeGenTestsSuite();

    return runner.run(testSuites);
}
