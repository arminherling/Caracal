#include <CaraTest.h>
#include "VirtualMachineTests.h"

int main(int argc, char* argv[])
{
    CaraTest::TestRunner runner{ argc, argv };
    auto testSuites = VirtualMachineTests();

    return runner.run(testSuites);
}
