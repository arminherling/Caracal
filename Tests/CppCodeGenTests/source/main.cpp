#include <AalTest.h>
#include "CppCodeGenTests.h"

int main(int argc, char* argv[])
{
    AalTest::TestRunner runner{ argc, argv };
    auto testSuites = CppCodeGenTestsSuite();

    runner.run(testSuites);

    return 0;
}
