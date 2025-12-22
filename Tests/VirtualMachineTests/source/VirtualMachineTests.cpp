#include <CaraTest.h>
//#include <Debug/ByteCodeDisassembler.h>
#include <iostream>
#include <Caracal/VirtualMachine/ByteCode.h>
#include <Caracal/VirtualMachine/ByteCodeAssembler.h>
#include <Caracal/VirtualMachine/Register.h>
#include <Caracal/VirtualMachine/VM.h>

using namespace CaraTest;
using namespace Caracal;

static void LoadBool(const std::string& testName, bool value)
{
    ByteCode code;
    ByteCodeAssembler assembler{ code };
    assembler.emitLoadBool(1, value);
    assembler.emitHalt();
    VM vm;

    auto startTime = std::chrono::high_resolution_clock::now();
    vm.run(code);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      run(): " << stringify(endTime - startTime) << std::endl;

    auto loadedValue = vm.getValue(1);
    CaraTest::isTrue(loadedValue.isBool());
    CaraTest::areEqual(loadedValue.asBool(), value);
}

static std::vector<std::tuple<std::string, bool>> LoadBool_Data()
{
    return {
        std::make_tuple(std::string("true"), true),
        std::make_tuple(std::string("false"), false),
    };
}

static void NotBool(const std::string& testName, bool value, bool expectedResult)
{
    ByteCode code;
    ByteCodeAssembler assembler{ code };
    assembler.emitLoadBool(1, value);
    assembler.emitNotBool(0, 1);
    assembler.emitHalt();
    VM vm;

    auto startTime = std::chrono::high_resolution_clock::now();
    vm.run(code);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      run(): " << stringify(endTime - startTime) << std::endl;

    auto loadedValue = vm.getValue(0);
    CaraTest::isTrue(loadedValue.isBool());
    CaraTest::areEqual(loadedValue.asBool(), expectedResult);
}

static std::vector<std::tuple<std::string, bool, bool>> NotBool_Data()
{
    return {
        std::make_tuple(std::string("!true = false"), true, false),
        std::make_tuple(std::string("!false = true"), false, true)
    };
}

static void EqualBool(const std::string& testName, bool lhs, bool rhs, bool expectedResult)
{
    ByteCode code;
    ByteCodeAssembler assembler{ code };
    assembler.emitLoadBool(1, lhs);
    assembler.emitLoadBool(2, rhs);
    assembler.emitEqualBool(0, 1, 2);
    assembler.emitHalt();
    VM vm;

    auto startTime = std::chrono::high_resolution_clock::now();
    vm.run(code);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      run(): " << stringify(endTime - startTime) << std::endl;

    auto loadedValue = vm.getValue(0);
    CaraTest::isTrue(loadedValue.isBool());
    CaraTest::areEqual(loadedValue.asBool(), expectedResult);
}

static std::vector<std::tuple<std::string, bool, bool, bool>> EqualBool_Data()
{
    return {
        std::make_tuple(std::string("true == false = false"), true, false, false),
        std::make_tuple(std::string("true == true = true"), true, true, true),
        std::make_tuple(std::string("false == false = true"), false, false, true)
    };
}

static void NotEqualBool(const std::string& testName, bool lhs, bool rhs, bool expectedResult)
{
    ByteCode code;
    ByteCodeAssembler assembler{ code };
    assembler.emitLoadBool(1, lhs);
    assembler.emitLoadBool(2, rhs);
    assembler.emitNotEqualBool(0, 1, 2);
    assembler.emitHalt();
    VM vm;

    auto startTime = std::chrono::high_resolution_clock::now();
    vm.run(code);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      run(): " << stringify(endTime - startTime) << std::endl;

    auto loadedValue = vm.getValue(0);
    CaraTest::isTrue(loadedValue.isBool());
    CaraTest::areEqual(loadedValue.asBool(), expectedResult);
}

static std::vector<std::tuple<std::string, bool, bool, bool>> NotEqualBool_Data()
{
    return {
        std::make_tuple(std::string("true != false = true"), true, false, true),
        std::make_tuple(std::string("true != true = false"), true, true, false),
        std::make_tuple(std::string("false != false = false"), false, false, false)
    };
}

static void LoadInt32(const std::string& testName, i32 value)
{
    ByteCode code;
    ByteCodeAssembler assembler{ code };
    assembler.emitLoadInt32(1, value);
    assembler.emitHalt();
    VM vm;

    auto startTime = std::chrono::high_resolution_clock::now();
    vm.run(code);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      run(): " << stringify(endTime - startTime) << std::endl;

    auto loadedValue = vm.getValue(1);
    CaraTest::isTrue(loadedValue.isInt32());
    CaraTest::areEqual(loadedValue.asInt32(), value);
}

static std::vector<std::tuple<std::string, i32>> LoadInt32_Data()
{
    return {
        std::make_tuple(std::string("123"), 123),
        std::make_tuple(std::string("-100000"), -100000)
    };
}

static void AddInt32(const std::string& testName, i32 lhs, i32 rhs, i32 expectedResult)
{
    ByteCode code;
    ByteCodeAssembler assembler{ code };
    assembler.emitLoadInt32(1, lhs);
    assembler.emitLoadInt32(2, rhs);
    assembler.emitAddInt32(0, 1, 2);
    assembler.emitHalt();
    VM vm;

    auto startTime = std::chrono::high_resolution_clock::now();
    vm.run(code);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      run(): " << stringify(endTime - startTime) << std::endl;

    auto loadedValue = vm.getValue(0);
    CaraTest::isTrue(loadedValue.isInt32());
    CaraTest::areEqual(loadedValue.asInt32(), expectedResult);
}

static std::vector<std::tuple<std::string, i32, i32, i32>> AddInt32_Data()
{
    return {
        std::make_tuple(std::string("10 + 100 = 110"), 10, 100, 110),
        std::make_tuple(std::string("-500 + -100 = -600"), -500, -100, -600)
    };
}

static void SubtractInt32(const std::string& testName, i32 lhs, i32 rhs, i32 expectedResult)
{
    ByteCode code;
    ByteCodeAssembler assembler{ code };
    assembler.emitLoadInt32(1, lhs);
    assembler.emitLoadInt32(2, rhs);
    assembler.emitSubtractInt32(0, 1, 2);
    assembler.emitHalt();
    VM vm;

    auto startTime = std::chrono::high_resolution_clock::now();
    vm.run(code);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      run(): " << stringify(endTime - startTime) << std::endl;

    auto loadedValue = vm.getValue(0);
    CaraTest::isTrue(loadedValue.isInt32());
    CaraTest::areEqual(loadedValue.asInt32(), expectedResult);
}

static std::vector<std::tuple<std::string, i32, i32, i32>> SubtractInt32_Data()
{
    return {
        std::make_tuple(std::string("10 - 100 = -90"), 10, 100, -90),
        std::make_tuple(std::string("-500 - -100 = -400"), -500, -100, -400)
    };
}

static void MultiplyInt32(const std::string& testName, i32 lhs, i32 rhs, i32 expectedResult)
{
    ByteCode code;
    ByteCodeAssembler assembler{ code };
    assembler.emitLoadInt32(1, lhs);
    assembler.emitLoadInt32(2, rhs);
    assembler.emitMultiplyInt32(0, 1, 2);
    assembler.emitHalt();
    VM vm;

    auto startTime = std::chrono::high_resolution_clock::now();
    vm.run(code);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      run(): " << stringify(endTime - startTime) << std::endl;

    auto loadedValue = vm.getValue(0);
    CaraTest::isTrue(loadedValue.isInt32());
    CaraTest::areEqual(loadedValue.asInt32(), expectedResult);
}

static std::vector<std::tuple<std::string, i32, i32, i32>> MultiplyInt32_Data()
{
    return {
        std::make_tuple(std::string("10 * 100 = 1000"), 10, 100, 1000),
        std::make_tuple(std::string("5 * -100 = -500"), 5, -100, -500)
    };
}

static void DivideInt32(const std::string& testName, i32 lhs, i32 rhs, i32 expectedResult)
{
    ByteCode code;
    ByteCodeAssembler assembler{ code };
    assembler.emitLoadInt32(1, lhs);
    assembler.emitLoadInt32(2, rhs);
    assembler.emitDivideInt32(0, 1, 2);
    assembler.emitHalt();
    VM vm;

    auto startTime = std::chrono::high_resolution_clock::now();
    vm.run(code);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      run(): " << stringify(endTime - startTime) << std::endl;

    auto loadedValue = vm.getValue(0);
    CaraTest::isTrue(loadedValue.isInt32());
    CaraTest::areEqual(loadedValue.asInt32(), expectedResult);
}

static std::vector<std::tuple<std::string, i32, i32, i32>> DivideInt32_Data()
{
    return {
        std::make_tuple(std::string("100 / 10 = 10"), 100, 10, 10),
        std::make_tuple(std::string("-100 / 5 = -20"), -100, 5, -20)
    };
}

static void NegateIn32(const std::string& testName, i32 value, i32 expectedResult)
{
    ByteCode code;
    ByteCodeAssembler assembler{ code };
    assembler.emitLoadInt32(1, value);
    assembler.emitNegateInt32(0, 1);
    assembler.emitHalt();
    VM vm;

    auto startTime = std::chrono::high_resolution_clock::now();
    vm.run(code);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      run(): " << stringify(endTime - startTime) << std::endl;

    auto loadedValue = vm.getValue(0);
    CaraTest::isTrue(loadedValue.isInt32());
    CaraTest::areEqual(loadedValue.asInt32(), expectedResult);
}

static std::vector<std::tuple<std::string, i32, i32>> NegateIn32_Data()
{
    return {
        std::make_tuple(std::string("-(10) = -10"), 10, -10),
        std::make_tuple(std::string("-(-100) = 100"), -100, 100),
        std::make_tuple(std::string("-(0) = 0"), 0, 0)
    };
}

static void EqualInt32(const std::string& testName, i32 lhs, i32 rhs, bool expectedResult)
{
    ByteCode code;
    ByteCodeAssembler assembler{ code };
    assembler.emitLoadInt32(1, lhs);
    assembler.emitLoadInt32(2, rhs);
    assembler.emitEqualInt32(0, 1, 2);
    assembler.emitHalt();
    VM vm;

    auto startTime = std::chrono::high_resolution_clock::now();
    vm.run(code);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      run(): " << stringify(endTime - startTime) << std::endl;

    auto loadedValue = vm.getValue(0);
    CaraTest::isTrue(loadedValue.isBool());
    CaraTest::areEqual(loadedValue.asBool(), expectedResult);
}

static std::vector<std::tuple<std::string, i32, i32, bool>> EqualInt32_Data()
{
    return {
        std::make_tuple(std::string("100 == 10 = false"), 100, 10, false),
        std::make_tuple(std::string("5 == 5 = true"), 5, 5, true)
    };
}

static void NotEqualInt32(const std::string& testName, i32 lhs, i32 rhs, bool expectedResult)
{
    ByteCode code;
    ByteCodeAssembler assembler{ code };
    assembler.emitLoadInt32(1, lhs);
    assembler.emitLoadInt32(2, rhs);
    assembler.emitNotEqualInt32(0, 1, 2);
    assembler.emitHalt();
    VM vm;

    auto startTime = std::chrono::high_resolution_clock::now();
    vm.run(code);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      run(): " << stringify(endTime - startTime) << std::endl;

    auto loadedValue = vm.getValue(0);
    CaraTest::isTrue(loadedValue.isBool());
    CaraTest::areEqual(loadedValue.asBool(), expectedResult);
}

static std::vector<std::tuple<std::string, i32, i32, bool>> NotEqualInt32_Data()
{
    return {
        std::make_tuple(std::string("100 != 10 = true"), 100, 10, true),
        std::make_tuple(std::string("5 != 5 = false"), 5, 5, false)
    };
}

static void GreaterInt32(const std::string& testName, i32 lhs, i32 rhs, bool expectedResult)
{
    ByteCode code;
    ByteCodeAssembler assembler{ code };
    assembler.emitLoadInt32(1, lhs);
    assembler.emitLoadInt32(2, rhs);
    assembler.emitGreaterInt32(0, 1, 2);
    assembler.emitHalt();
    VM vm;

    auto startTime = std::chrono::high_resolution_clock::now();
    vm.run(code);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      run(): " << stringify(endTime - startTime) << std::endl;

    auto loadedValue = vm.getValue(0);
    CaraTest::isTrue(loadedValue.isBool());
    CaraTest::areEqual(loadedValue.asBool(), expectedResult);
}

static std::vector<std::tuple<std::string, i32, i32, bool>> GreaterInt32_Data()
{
    return {
        std::make_tuple(std::string("100 > 10 = true"), 100, 10, true),
        std::make_tuple(std::string("5 > 5 = false"), 5, 5, false),
        std::make_tuple(std::string("1 > 2 = false"), 1, 2, false)
    };
}

static void GreaterOrEqualInt32(const std::string& testName, i32 lhs, i32 rhs, bool expectedResult)
{
    ByteCode code;
    ByteCodeAssembler assembler{ code };
    assembler.emitLoadInt32(1, lhs);
    assembler.emitLoadInt32(2, rhs);
    assembler.emitGreaterOrEqualInt32(0, 1, 2);
    assembler.emitHalt();
    VM vm;

    auto startTime = std::chrono::high_resolution_clock::now();
    vm.run(code);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      run(): " << stringify(endTime - startTime) << std::endl;

    auto loadedValue = vm.getValue(0);
    CaraTest::isTrue(loadedValue.isBool());
    CaraTest::areEqual(loadedValue.asBool(), expectedResult);
}

static std::vector<std::tuple<std::string, i32, i32, bool>> GreaterOrEqualInt32_Data()
{
    return {
        std::make_tuple(std::string("100 >= 10 = true"), 100, 10, true),
        std::make_tuple(std::string("5 >= 5 = true"), 5, 5, true),
        std::make_tuple(std::string("1 >= 2 = false"), 1, 2, false)
    };
}

static void LessInt32(const std::string& testName, i32 lhs, i32 rhs, bool expectedResult)
{
    ByteCode code;
    ByteCodeAssembler assembler{ code };
    assembler.emitLoadInt32(1, lhs);
    assembler.emitLoadInt32(2, rhs);
    assembler.emitLessInt32(0, 1, 2);
    assembler.emitHalt();
    VM vm;

    auto startTime = std::chrono::high_resolution_clock::now();
    vm.run(code);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      run(): " << stringify(endTime - startTime) << std::endl;

    auto loadedValue = vm.getValue(0);
    CaraTest::isTrue(loadedValue.isBool());
    CaraTest::areEqual(loadedValue.asBool(), expectedResult);
}

static std::vector<std::tuple<std::string, i32, i32, bool>> LessInt32_Data()
{
    return {
        std::make_tuple(std::string("100 < 10 = false"), 100, 10, false),
        std::make_tuple(std::string("5 < 5 = false"), 5, 5, false),
        std::make_tuple(std::string("1 < 2 = true"), 1, 2, true)
    };
}

static void LessOrEqualInt32(const std::string& testName, i32 lhs, i32 rhs, bool expectedResult)
{
    ByteCode code;
    ByteCodeAssembler assembler{ code };
    assembler.emitLoadInt32(1, lhs);
    assembler.emitLoadInt32(2, rhs);
    assembler.emitLessOrEqualInt32(0, 1, 2);
    assembler.emitHalt();
    VM vm;

    auto startTime = std::chrono::high_resolution_clock::now();
    vm.run(code);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      run(): " << stringify(endTime - startTime) << std::endl;

    auto loadedValue = vm.getValue(0);
    CaraTest::isTrue(loadedValue.isBool());
    CaraTest::areEqual(loadedValue.asBool(), expectedResult);
}

static std::vector<std::tuple<std::string, i32, i32, bool>> LessOrEqualInt32_Data()
{
    return {
        std::make_tuple(std::string("100 <= 10 = false"), 100, 10, false),
        std::make_tuple(std::string("5 <= 5 = true"), 5, 5, true),
        std::make_tuple(std::string("1 <= 2 = true"), 1, 2, true)
    };
}

static void MoveBool(const std::string& testName, bool value)
{
    ByteCode code;
    ByteCodeAssembler assembler{ code };
    assembler.emitLoadBool(1, value);
    assembler.emitMove(0, 1);
    assembler.emitHalt();
    VM vm;

    auto startTime = std::chrono::high_resolution_clock::now();
    vm.run(code);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      run(): " << stringify(endTime - startTime) << std::endl;

    auto loadedValue = vm.getValue(0);
    CaraTest::areEqual(loadedValue.asBool(), value);
}

static std::vector<std::tuple<std::string, bool>> MoveBool_Data()
{
    return {
        std::make_tuple(std::string("true"), true),
        std::make_tuple(std::string("false"), false)
    };
}

static void MoveInt32(const std::string& testName, i32 value)
{
    ByteCode code;
    ByteCodeAssembler assembler{ code };
    assembler.emitLoadInt32(1, value);
    assembler.emitMove(0, 1);
    assembler.emitHalt();
    VM vm;

    auto startTime = std::chrono::high_resolution_clock::now();
    vm.run(code);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      run(): " << stringify(endTime - startTime) << std::endl;

    auto loadedValue = vm.getValue(0);
    CaraTest::areEqual(loadedValue.asInt32(), value);
}

static std::vector<std::tuple<std::string, i32>> MoveInt32_Data()
{
    return {
        std::make_tuple(std::string("0"), 0),
        std::make_tuple(std::string("100"), 100),
        std::make_tuple(std::string("-99"), -99)
    };
}

static void Jump()
{
    ByteCode code;
    ByteCodeAssembler assembler{ code };
    assembler.emitLoadInt32(0, 10);
    auto jumpIndex = assembler.emitJump();
    assembler.emitLoadInt32(0, 20);
    auto endLabel = assembler.createLabel();
    assembler.patchJump(jumpIndex, endLabel);
    assembler.emitHalt();
    VM vm;

    auto startTime = std::chrono::high_resolution_clock::now();
    vm.run(code);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      run(): " << stringify(endTime - startTime) << std::endl;

    auto loadedValue = vm.getValue(0);
    CaraTest::isTrue(loadedValue.isInt32());
    CaraTest::areEqual(loadedValue.asInt32(), 10);
}

static void JumpIfTrue(const std::string& testName, bool condition, i32 expectedResult)
{
    ByteCode code;
    ByteCodeAssembler assembler{ code };
    assembler.emitLoadBool(0, condition);
    assembler.emitLoadInt32(1, 10);
    auto jumpIndex = assembler.emitJumpIfTrue(0);
    assembler.emitLoadInt32(1, 20);
    auto endLabel = assembler.createLabel();
    assembler.patchJump(jumpIndex, endLabel);
    assembler.emitHalt();
    VM vm;

    auto startTime = std::chrono::high_resolution_clock::now();
    vm.run(code);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      run(): " << stringify(endTime - startTime) << std::endl;

    auto loadedValue = vm.getValue(1);
    CaraTest::isTrue(loadedValue.isInt32());
    CaraTest::areEqual(loadedValue.asInt32(), expectedResult);
}

static std::vector<std::tuple<std::string, bool, i32>> JumpIfTrue_Data()
{
    return {
        std::make_tuple(std::string("false -> 20"), false, 20),
        std::make_tuple(std::string("true -> 10"), true, 10)
    };
}

static void JumpIfFalse(const std::string& testName, bool condition, i32 expectedResult)
{
    ByteCode code;
    ByteCodeAssembler assembler{ code };
    assembler.emitLoadBool(0, condition);
    assembler.emitLoadInt32(1, 10);
    auto jumpIndex = assembler.emitJumpIfFalse(0);
    assembler.emitLoadInt32(1, 20);
    auto endLabel = assembler.createLabel();
    assembler.patchJump(jumpIndex, endLabel);
    assembler.emitHalt();
    VM vm;

    auto startTime = std::chrono::high_resolution_clock::now();
    vm.run(code);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      run(): " << stringify(endTime - startTime) << std::endl;

    auto loadedValue = vm.getValue(1);
    CaraTest::isTrue(loadedValue.isInt32());
    CaraTest::areEqual(loadedValue.asInt32(), expectedResult);
}

static std::vector<std::tuple<std::string, bool, i32>> JumpIfFalse_Data()
{
    return {
        std::make_tuple(std::string("false -> 10"), false, 10),
        std::make_tuple(std::string("true -> 20"), true, 20)
    };
}

static void PrintBool(const std::string& testName, bool value)
{
    ByteCode code;
    ByteCodeAssembler assembler{ code };
    assembler.emitLoadBool(0, value);
    assembler.emitPrintBool(0);
    assembler.emitPrintNewLine();
    assembler.emitHalt();
    VM vm;

    auto startTime = std::chrono::high_resolution_clock::now();
    vm.run(code);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      run(): " << stringify(endTime - startTime) << std::endl;
}

static std::vector<std::tuple<std::string, bool>> PrintBool_Data()
{
    return {
        std::make_tuple(std::string("true"), true),
        std::make_tuple(std::string("false"), false)
    };
}

static void PrintInt32(const std::string& testName, i32 value)
{
    ByteCode code;
    ByteCodeAssembler assembler{ code };
    assembler.emitLoadInt32(0, value);
    assembler.emitPrintInt32(0);
    assembler.emitPrintNewLine();
    assembler.emitHalt();
    VM vm;

    auto startTime = std::chrono::high_resolution_clock::now();
    vm.run(code);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      run(): " << stringify(endTime - startTime) << std::endl;
}

static std::vector<std::tuple<std::string, i32>> PrintInt32_Data()
{
    return {
        std::make_tuple(std::string("0"), 0),
        std::make_tuple(std::string("12345"), 12345)
    };
}

static void PrintNewLine()
{
    ByteCode code;
    ByteCodeAssembler assembler{ code };
    assembler.emitPrintNewLine();
    assembler.emitHalt();
    VM vm;

    auto startTime = std::chrono::high_resolution_clock::now();
    vm.run(code);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      run(): " << stringify(endTime - startTime) << std::endl;
}

static void LoweredWhileLoop()
{
    // int i = 0;
    // while (i < 10)
    // {
    //     i = i + 1;
    // }

    ByteCode code;
    ByteCodeAssembler assembler{ code };
    assembler.emitLoadInt32(0, 0);                     //  int i = 0
    assembler.emitLoadInt32(1, 10);                    //  int literal 10
    auto beginLabel = assembler.createLabel();         // begin:
    assembler.emitLessInt32(2, 0, 1);                  //  i < 10
    auto endJumpIndex = assembler.emitJumpIfFalse(2);  //  goto end if false
    assembler.emitLoadInt32(3, 1);                     //  int literal 1
    assembler.emitAddInt32(0, 0, 3);                   //  i = i + 1
    assembler.emitJump(beginLabel);                    //  goto begin
    auto endLabel = assembler.createLabel();           // end:
    assembler.patchJump(endJumpIndex, endLabel);
    assembler.emitHalt();
    VM vm;

    auto startTime = std::chrono::high_resolution_clock::now();
    vm.run(code);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      run(): " << stringify(endTime - startTime) << std::endl;

    auto loadedValue = vm.getValue(0);
    CaraTest::isTrue(loadedValue.isInt32());
    CaraTest::areEqual(loadedValue.asInt32(), 10);
}

static void FunctionDeclarationTest()
{
    auto functionName = std::string("function");
    auto returnValues = 1;
    auto parameterValues = 2;

    ByteCode code;
    ByteCodeAssembler assembler{ code };
    assembler.declareFunction(functionName, returnValues, parameterValues);

    auto optDeclaration = code.getFunctionDeclaration(functionName);
    CaraTest::isTrue(optDeclaration.has_value());
    auto& declaration = optDeclaration.value();
    CaraTest::areEqual(declaration.name, functionName);
    CaraTest::areEqual(declaration.returnValues, returnValues);
    CaraTest::areEqual(declaration.parameterValues, parameterValues);
}

static void FunctionCall()
{
    auto addFunctionName = std::string("add");

    ByteCode code;
    ByteCodeAssembler assembler{ code };
    assembler.emitLoadInt32(0, 10);
    assembler.emitLoadInt32(1, 100);
    // register 2 is return value of add function
    assembler.emitLoadInt32(3, 25);
    assembler.emitLoadInt32(4, 50);
    assembler.emitFunctionCall(addFunctionName, 2);
    assembler.emitPrintInt32(2);
    assembler.emitPrintNewLine();
    assembler.emitHalt();

    assembler.declareFunction(addFunctionName, 1, 2);
    assembler.emitAddInt32(0, 1, 2);
    assembler.emitHalt();
    assembler.patchFunctionCalls();
    VM vm;

    auto startTime = std::chrono::high_resolution_clock::now();
    vm.run(code);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      run(): " << stringify(endTime - startTime) << std::endl;
}

static void Fib20()
{
    //int fib(int x)
    //{
    //    if (x < 2)
    //    {
    //        return x;
    //    }
    //    else
    //    {
    //        return fib(x - 1) + fib(x - 2);
    //    }
    //}

    auto fibFunctionName = std::string("fib");

    ByteCode code;
    ByteCodeAssembler assembler{ code };
    assembler.emitLoadInt32(1, 20); // set fib parameter 20
    assembler.emitFunctionCall(fibFunctionName, 0);

    assembler.emitPrintInt32(0); // print the result
    assembler.emitPrintNewLine();
    assembler.emitHalt();

    assembler.declareFunction(fibFunctionName, 1, 1);
    assembler.emitLoadInt32(2, 1);  // 1 literal
    assembler.emitLoadInt32(3, 2);  // 2 literal

    assembler.emitLessInt32(4, 1, 3); // x == 0
    auto elseJumpIndex = assembler.emitJumpIfFalse(4);
    assembler.emitMove(0, 1);   // move x to the return value
    assembler.emitHalt();

    auto elseBranchLabel = assembler.createLabel();
    // else branch
    // 5 return for fib(x-1)
    assembler.emitSubtractInt32(6, 1, 2); // x-1
    assembler.emitFunctionCall(fibFunctionName, 5);

    // 7 return for fib(x-2)
    assembler.emitSubtractInt32(8, 1, 3); // x-2
    assembler.emitFunctionCall(fibFunctionName, 7);

    assembler.emitAddInt32(0, 5, 7);    // add both results
    assembler.emitHalt();

    assembler.patchJump(elseJumpIndex, elseBranchLabel);
    assembler.patchFunctionCalls();
    VM vm;

    auto startTime = std::chrono::high_resolution_clock::now();
    vm.run(code);

    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "      run(): " << stringify(endTime - startTime) << std::endl;
}

static auto tests =
{
    CaraTest::addTest("LoadBool", LoadBool, LoadBool_Data),
    CaraTest::addTest("NotBool", NotBool, NotBool_Data),
    CaraTest::addTest("EqualBool", EqualBool, EqualBool_Data),
    CaraTest::addTest("NotEqualBool", NotEqualBool, NotEqualBool_Data),
    CaraTest::addTest("LoadInt32", LoadInt32, LoadInt32_Data),
    CaraTest::addTest("AddInt32", AddInt32, AddInt32_Data),
    CaraTest::addTest("SubtractInt32", SubtractInt32, SubtractInt32_Data),
    CaraTest::addTest("MultiplyInt32", MultiplyInt32, MultiplyInt32_Data),
    CaraTest::addTest("DivideInt32", DivideInt32, DivideInt32_Data),
    CaraTest::addTest("NegateIn32", NegateIn32, NegateIn32_Data),
    CaraTest::addTest("EqualInt32", EqualInt32, EqualInt32_Data),
    CaraTest::addTest("NotEqualInt32", NotEqualInt32, NotEqualInt32_Data),
    CaraTest::addTest("GreaterInt32", GreaterInt32, GreaterInt32_Data),
    CaraTest::addTest("GreaterOrEqualInt32", GreaterOrEqualInt32, GreaterOrEqualInt32_Data),
    CaraTest::addTest("LessInt32", LessInt32, LessInt32_Data),
    CaraTest::addTest("LessOrEqualInt32", LessOrEqualInt32, LessOrEqualInt32_Data),
    CaraTest::addTest("MoveBool", MoveBool, MoveBool_Data),
    CaraTest::addTest("MoveInt32", MoveInt32, MoveInt32_Data),
    CaraTest::addTest("Jump", Jump),
    CaraTest::addTest("JumpIfTrue", JumpIfTrue, JumpIfTrue_Data),
    CaraTest::addTest("JumpIfFalse", JumpIfFalse, JumpIfFalse_Data),
    CaraTest::addTest("PrintBool", PrintBool, PrintBool_Data),
    CaraTest::addTest("PrintInt32", PrintInt32, PrintInt32_Data),
    CaraTest::addTest("PrintNewLine", PrintNewLine),
    CaraTest::addTest("LoweredWhileLoop", LoweredWhileLoop),
    CaraTest::addTest("FunctionDeclaration", FunctionDeclarationTest),
    CaraTest::addTest("FunctionCall", FunctionCall),
    CaraTest::addTest("Fib20", Fib20),
};
