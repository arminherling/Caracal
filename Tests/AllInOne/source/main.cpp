#include <CaraTest.h>
#include "../../CppCodeGenTests/source/CppCodeGenTests.h"
#include "../../LexerTests/source/LexerTests.h"
#include "../../ParserTests/source/ParserTests.h"
#include "../../TypeCheckerTests/source/TypeCheckerTests.h"
#include "../../SourceLocationTests/source/SourceLocationTests.h"
#include "../../VirtualMachineTests/source/VirtualMachineTests.h"

int main(int argc, char* argv[])
{
    CaraTest::TestRunner runner{ argc, argv };
    QList<CaraTest::TestSuite> testSuites{};
    testSuites << LexerTestsSuite();
    testSuites << ParserTestsSuite();
    testSuites << TypeCheckerTestsSuite();
    testSuites << CppCodeGenTestsSuite();
    testSuites << SourceLocationTests();
    testSuites << VirtualMachineTests();

    runner.run(testSuites);

    return 0;
}