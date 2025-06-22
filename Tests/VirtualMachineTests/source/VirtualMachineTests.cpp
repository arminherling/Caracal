#include <CaraTest.h>
#include <Debug/ByteCodeDisassembler.h>
#include <iostream>
#include <VirtualMachine/ByteCode.h>
#include <VirtualMachine/ByteCodeAssembler.h>
#include <VirtualMachine/Register.h>
#include <VirtualMachine/VM.h>

using namespace CaraTest;

namespace
{
    void LoadBool(const QString& testName, bool value)
    {
        ByteCode code;
        ByteCodeAssembler assembler{ code };
        assembler.emitLoadBool(1, value);
        assembler.emitHalt();
        VM vm;

        auto startTime = std::chrono::high_resolution_clock::now();
        vm.run(code);

        auto endTime = std::chrono::high_resolution_clock::now();
        std::cout << "      run(): " << Stringify(endTime - startTime).toStdString() << std::endl;

        auto loadedValue = vm.getValue(1);
        CaraTest::IsTrue(loadedValue.isBool());
        CaraTest::AreEqual(loadedValue.asBool(), value);
    }

    QList<std::tuple<QString, bool>> LoadBool_Data()
    {
        return {
            std::make_tuple(QString("true"), true),
            std::make_tuple(QString("false"), false),
        };
    }

    void NotBool(const QString& testName, bool value, bool expectedResult)
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
        std::cout << "      run(): " << Stringify(endTime - startTime).toStdString() << std::endl;

        auto loadedValue = vm.getValue(0);
        CaraTest::IsTrue(loadedValue.isBool());
        CaraTest::AreEqual(loadedValue.asBool(), expectedResult);
    }

    QList<std::tuple<QString, bool, bool>> NotBool_Data()
    {
        return {
            std::make_tuple(QString("!true = false"), true, false),
            std::make_tuple(QString("!false = true"), false, true)
        };
    }

    void EqualBool(const QString& testName, bool lhs, bool rhs, bool expectedResult)
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
        std::cout << "      run(): " << Stringify(endTime - startTime).toStdString() << std::endl;

        auto loadedValue = vm.getValue(0);
        CaraTest::IsTrue(loadedValue.isBool());
        CaraTest::AreEqual(loadedValue.asBool(), expectedResult);
    }

    QList<std::tuple<QString, bool, bool, bool>> EqualBool_Data()
    {
        return {
            std::make_tuple(QString("true == false = false"), true, false, false),
            std::make_tuple(QString("true == true = true"), true, true, true),
            std::make_tuple(QString("false == false = true"), false, false, true)
        };
    }

    void NotEqualBool(const QString& testName, bool lhs, bool rhs, bool expectedResult)
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
        std::cout << "      run(): " << Stringify(endTime - startTime).toStdString() << std::endl;

        auto loadedValue = vm.getValue(0);
        CaraTest::IsTrue(loadedValue.isBool());
        CaraTest::AreEqual(loadedValue.asBool(), expectedResult);
    }

    QList<std::tuple<QString, bool, bool, bool>> NotEqualBool_Data()
    {
        return {
            std::make_tuple(QString("true != false = true"), true, false, true),
            std::make_tuple(QString("true != true = false"), true, true, false),
            std::make_tuple(QString("false != false = false"), false, false, false)
        };
    }

    void LoadInt32(const QString& testName, i32 value)
    {
        ByteCode code;
        ByteCodeAssembler assembler{ code };
        assembler.emitLoadInt32(1, value);
        assembler.emitHalt();
        VM vm;

        auto startTime = std::chrono::high_resolution_clock::now();
        vm.run(code);

        auto endTime = std::chrono::high_resolution_clock::now();
        std::cout << "      run(): " << Stringify(endTime - startTime).toStdString() << std::endl;

        auto loadedValue = vm.getValue(1);
        CaraTest::IsTrue(loadedValue.isInt32());
        CaraTest::AreEqual(loadedValue.asInt32(), value);
    }

    QList<std::tuple<QString, i32>> LoadInt32_Data()
    {
        return {
            std::make_tuple(QString("123"), 123),
            std::make_tuple(QString("-100000"), -100000)
        };
    }

    void AddInt32(const QString& testName, i32 lhs, i32 rhs, i32 expectedResult)
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
        std::cout << "      run(): " << Stringify(endTime - startTime).toStdString() << std::endl;

        auto loadedValue = vm.getValue(0);
        CaraTest::IsTrue(loadedValue.isInt32());
        CaraTest::AreEqual(loadedValue.asInt32(), expectedResult);
    }

    QList<std::tuple<QString, i32, i32, i32>> AddInt32_Data()
    {
        return {
            std::make_tuple(QString("10 + 100 = 110"), 10, 100, 110),
            std::make_tuple(QString("-500 + -100 = -600"), -500, -100, -600)
        };
    }

    void SubtractInt32(const QString& testName, i32 lhs, i32 rhs, i32 expectedResult)
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
        std::cout << "      run(): " << Stringify(endTime - startTime).toStdString() << std::endl;

        auto loadedValue = vm.getValue(0);
        CaraTest::IsTrue(loadedValue.isInt32());
        CaraTest::AreEqual(loadedValue.asInt32(), expectedResult);
    }

    QList<std::tuple<QString, i32, i32, i32>> SubtractInt32_Data()
    {
        return {
            std::make_tuple(QString("10 - 100 = -90"), 10, 100, -90),
            std::make_tuple(QString("-500 - -100 = -400"), -500, -100, -400)
        };
    }

    void MultiplyInt32(const QString& testName, i32 lhs, i32 rhs, i32 expectedResult)
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
        std::cout << "      run(): " << Stringify(endTime - startTime).toStdString() << std::endl;

        auto loadedValue = vm.getValue(0);
        CaraTest::IsTrue(loadedValue.isInt32());
        CaraTest::AreEqual(loadedValue.asInt32(), expectedResult);
    }

    QList<std::tuple<QString, i32, i32, i32>> MultiplyInt32_Data()
    {
        return {
            std::make_tuple(QString("10 * 100 = 1000"), 10, 100, 1000),
            std::make_tuple(QString("5 * -100 = -500"), 5, -100, -500)
        };
    }

    void DivideInt32(const QString& testName, i32 lhs, i32 rhs, i32 expectedResult)
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
        std::cout << "      run(): " << Stringify(endTime - startTime).toStdString() << std::endl;

        auto loadedValue = vm.getValue(0);
        CaraTest::IsTrue(loadedValue.isInt32());
        CaraTest::AreEqual(loadedValue.asInt32(), expectedResult);
    }

    QList<std::tuple<QString, i32, i32, i32>> DivideInt32_Data()
    {
        return {
            std::make_tuple(QString("100 / 10 = 10"), 100, 10, 10),
            std::make_tuple(QString("-100 / 5 = -20"), -100, 5, -20)
        };
    }

    void NegateIn32(const QString& testName, i32 value, i32 expectedResult)
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
        std::cout << "      run(): " << Stringify(endTime - startTime).toStdString() << std::endl;

        auto loadedValue = vm.getValue(0);
        CaraTest::IsTrue(loadedValue.isInt32());
        CaraTest::AreEqual(loadedValue.asInt32(), expectedResult);
    }

    QList<std::tuple<QString, i32, i32>> NegateIn32_Data()
    {
        return {
            std::make_tuple(QString("-(10) = -10"), 10, -10),
            std::make_tuple(QString("-(-100) = 100"), -100, 100),
            std::make_tuple(QString("-(0) = 0"), 0, 0)
        };
    }

    void EqualInt32(const QString& testName, i32 lhs, i32 rhs, bool expectedResult)
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
        std::cout << "      run(): " << Stringify(endTime - startTime).toStdString() << std::endl;

        auto loadedValue = vm.getValue(0);
        CaraTest::IsTrue(loadedValue.isBool());
        CaraTest::AreEqual(loadedValue.asBool(), expectedResult);
    }

    QList<std::tuple<QString, i32, i32, bool>> EqualInt32_Data()
    {
        return {
            std::make_tuple(QString("100 == 10 = false"), 100, 10, false),
            std::make_tuple(QString("5 == 5 = true"), 5, 5, true)
        };
    }

    void NotEqualInt32(const QString& testName, i32 lhs, i32 rhs, bool expectedResult)
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
        std::cout << "      run(): " << Stringify(endTime - startTime).toStdString() << std::endl;

        auto loadedValue = vm.getValue(0);
        CaraTest::IsTrue(loadedValue.isBool());
        CaraTest::AreEqual(loadedValue.asBool(), expectedResult);
    }

    QList<std::tuple<QString, i32, i32, bool>> NotEqualInt32_Data()
    {
        return {
            std::make_tuple(QString("100 != 10 = true"), 100, 10, true),
            std::make_tuple(QString("5 != 5 = false"), 5, 5, false)
        };
    }

    void GreaterInt32(const QString& testName, i32 lhs, i32 rhs, bool expectedResult)
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
        std::cout << "      run(): " << Stringify(endTime - startTime).toStdString() << std::endl;

        auto loadedValue = vm.getValue(0);
        CaraTest::IsTrue(loadedValue.isBool());
        CaraTest::AreEqual(loadedValue.asBool(), expectedResult);
    }

    QList<std::tuple<QString, i32, i32, bool>> GreaterInt32_Data()
    {
        return {
            std::make_tuple(QString("100 > 10 = true"), 100, 10, true),
            std::make_tuple(QString("5 > 5 = false"), 5, 5, false),
            std::make_tuple(QString("1 > 2 = false"), 1, 2, false)
        };
    }

    void GreaterOrEqualInt32(const QString& testName, i32 lhs, i32 rhs, bool expectedResult)
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
        std::cout << "      run(): " << Stringify(endTime - startTime).toStdString() << std::endl;

        auto loadedValue = vm.getValue(0);
        CaraTest::IsTrue(loadedValue.isBool());
        CaraTest::AreEqual(loadedValue.asBool(), expectedResult);
    }

    QList<std::tuple<QString, i32, i32, bool>> GreaterOrEqualInt32_Data()
    {
        return {
            std::make_tuple(QString("100 >= 10 = true"), 100, 10, true),
            std::make_tuple(QString("5 >= 5 = true"), 5, 5, true),
            std::make_tuple(QString("1 >= 2 = false"), 1, 2, false)
        };
    }

    void LessInt32(const QString& testName, i32 lhs, i32 rhs, bool expectedResult)
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
        std::cout << "      run(): " << Stringify(endTime - startTime).toStdString() << std::endl;

        auto loadedValue = vm.getValue(0);
        CaraTest::IsTrue(loadedValue.isBool());
        CaraTest::AreEqual(loadedValue.asBool(), expectedResult);
    }

    QList<std::tuple<QString, i32, i32, bool>> LessInt32_Data()
    {
        return {
            std::make_tuple(QString("100 < 10 = false"), 100, 10, false),
            std::make_tuple(QString("5 < 5 = false"), 5, 5, false),
            std::make_tuple(QString("1 < 2 = true"), 1, 2, true)
        };
    }

    void LessOrEqualInt32(const QString& testName, i32 lhs, i32 rhs, bool expectedResult)
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
        std::cout << "      run(): " << Stringify(endTime - startTime).toStdString() << std::endl;

        auto loadedValue = vm.getValue(0);
        CaraTest::IsTrue(loadedValue.isBool());
        CaraTest::AreEqual(loadedValue.asBool(), expectedResult);
    }

    QList<std::tuple<QString, i32, i32, bool>> LessOrEqualInt32_Data()
    {
        return {
            std::make_tuple(QString("100 <= 10 = false"), 100, 10, false),
            std::make_tuple(QString("5 <= 5 = true"), 5, 5, true),
            std::make_tuple(QString("1 <= 2 = true"), 1, 2, true)
        };
    }

    void MoveBool(const QString& testName, bool value)
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
        std::cout << "      run(): " << Stringify(endTime - startTime).toStdString() << std::endl;

        auto loadedValue = vm.getValue(0);
        CaraTest::AreEqual(loadedValue.asBool(), value);
    }

    QList<std::tuple<QString, bool>> MoveBool_Data()
    {
        return {
            std::make_tuple(QString("true"), true),
            std::make_tuple(QString("false"), false)
        };
    }

    void MoveInt32(const QString& testName, i32 value)
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
        std::cout << "      run(): " << Stringify(endTime - startTime).toStdString() << std::endl;

        auto loadedValue = vm.getValue(0);
        CaraTest::AreEqual(loadedValue.asInt32(), value);
    }

    QList<std::tuple<QString, i32>> MoveInt32_Data()
    {
        return {
            std::make_tuple(QString("0"), 0),
            std::make_tuple(QString("100"), 100),
            std::make_tuple(QString("-99"), -99)
        };
    }

    void Jump()
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
        std::cout << "      run(): " << Stringify(endTime - startTime).toStdString() << std::endl;

        auto loadedValue = vm.getValue(0);
        CaraTest::IsTrue(loadedValue.isInt32());
        CaraTest::AreEqual(loadedValue.asInt32(), 10);
    }

    void JumpIfTrue(const QString& testName, bool condition, i32 expectedResult)
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
        std::cout << "      run(): " << Stringify(endTime - startTime).toStdString() << std::endl;

        auto loadedValue = vm.getValue(1);
        CaraTest::IsTrue(loadedValue.isInt32());
        CaraTest::AreEqual(loadedValue.asInt32(), expectedResult);
    }

    QList<std::tuple<QString, bool, i32>> JumpIfTrue_Data()
    {
        return {
            std::make_tuple(QString("false -> 20"), false, 20),
            std::make_tuple(QString("true -> 10"), true, 10)
        };
    }

    void JumpIfFalse(const QString& testName, bool condition, i32 expectedResult)
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
        std::cout << "      run(): " << Stringify(endTime - startTime).toStdString() << std::endl;

        auto loadedValue = vm.getValue(1);
        CaraTest::IsTrue(loadedValue.isInt32());
        CaraTest::AreEqual(loadedValue.asInt32(), expectedResult);
    }

    QList<std::tuple<QString, bool, i32>> JumpIfFalse_Data()
    {
        return {
            std::make_tuple(QString("false -> 10"), false, 10),
            std::make_tuple(QString("true -> 20"), true, 20)
        };
    }

    void PrintBool(const QString& testName, bool value)
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
        std::cout << "      run(): " << Stringify(endTime - startTime).toStdString() << std::endl;
    }

    QList<std::tuple<QString, bool>> PrintBool_Data()
    {
        return {
            std::make_tuple(QString("true"), true),
            std::make_tuple(QString("false"), false)
        };
    }

    void PrintInt32(const QString& testName, i32 value)
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
        std::cout << "      run(): " << Stringify(endTime - startTime).toStdString() << std::endl;
    }

    QList<std::tuple<QString, i32>> PrintInt32_Data()
    {
        return {
            std::make_tuple(QString("0"), 0),
            std::make_tuple(QString("12345"), 12345)
        };
    }

    void PrintNewLine()
    {
        ByteCode code;
        ByteCodeAssembler assembler{ code };
        assembler.emitPrintNewLine();
        assembler.emitHalt();
        VM vm;

        auto startTime = std::chrono::high_resolution_clock::now();
        vm.run(code);

        auto endTime = std::chrono::high_resolution_clock::now();
        std::cout << "      run(): " << Stringify(endTime - startTime).toStdString() << std::endl;
    }

    void LoweredWhileLoop()
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
        std::cout << "      run(): " << Stringify(endTime - startTime).toStdString() << std::endl;

        auto loadedValue = vm.getValue(0);
        CaraTest::IsTrue(loadedValue.isInt32());
        CaraTest::AreEqual(loadedValue.asInt32(), 10);
    }

    void FunctionDeclarationTest()
    {
        auto functionName = QString("function");
        auto returnValues = 1;
        auto parameterValues = 2;

        ByteCode code;
        ByteCodeAssembler assembler{ code };
        assembler.declareFunction(functionName, returnValues, parameterValues);

        auto optDeclaration = code.getFunctionDeclaration(functionName);
        CaraTest::IsTrue(optDeclaration.has_value());
        auto declaration = optDeclaration.value();
        CaraTest::AreEqual(declaration.name, functionName);
        CaraTest::AreEqual(declaration.returnValues, returnValues);
        CaraTest::AreEqual(declaration.parameterValues, parameterValues);
    }

    void FunctionCall()
    {
        auto addFunctionName = QString("add");

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
        std::cout << "      run(): " << Stringify(endTime - startTime).toStdString() << std::endl;
    }

    void Fib20()
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

        auto fibFunctionName = QString("fib");

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
        std::cout << "      run(): " << Stringify(endTime - startTime).toStdString() << std::endl;
    }
}

CaraTest::TestSuite VirtualMachineTests()
{
    CaraTest::TestSuite suite{};

    suite.add(QString("LoadBool"), LoadBool, LoadBool_Data);
    suite.add(QString("NotBool"), NotBool, NotBool_Data);
    suite.add(QString("EqualBool"), EqualBool, EqualBool_Data);
    suite.add(QString("NotEqualBool"), NotEqualBool, NotEqualBool_Data);
    suite.add(QString("LoadInt32"), LoadInt32, LoadInt32_Data);
    suite.add(QString("AddInt32"), AddInt32, AddInt32_Data);
    suite.add(QString("SubtractInt32"), SubtractInt32, SubtractInt32_Data);
    suite.add(QString("MultiplyInt32"), MultiplyInt32, MultiplyInt32_Data);
    suite.add(QString("DivideInt32"), DivideInt32, DivideInt32_Data);
    suite.add(QString("NegateIn32"), NegateIn32, NegateIn32_Data);
    suite.add(QString("EqualInt32"), EqualInt32, EqualInt32_Data);
    suite.add(QString("NotEqualInt32"), NotEqualInt32, NotEqualInt32_Data);
    suite.add(QString("GreaterInt32"), GreaterInt32, GreaterInt32_Data);
    suite.add(QString("GreaterOrEqualInt32"), GreaterOrEqualInt32, GreaterOrEqualInt32_Data);
    suite.add(QString("LessInt32"), LessInt32, LessInt32_Data);
    suite.add(QString("LessOrEqualInt32"), LessOrEqualInt32, LessOrEqualInt32_Data);
    suite.add(QString("MoveBool"), MoveBool, MoveBool_Data);
    suite.add(QString("MoveInt32"), MoveInt32, MoveInt32_Data);
    suite.add(QString("Jump"), Jump);
    suite.add(QString("JumpIfTrue"), JumpIfTrue, JumpIfTrue_Data);
    suite.add(QString("JumpIfFalse"), JumpIfFalse, JumpIfFalse_Data);
    suite.add(QString("PrintBool"), PrintBool, PrintBool_Data);
    suite.add(QString("PrintInt32"), PrintInt32, PrintInt32_Data);
    suite.add(QString("PrintNewLine"), PrintNewLine);
    suite.add(QString("LoweredWhileLoop"), LoweredWhileLoop);
    suite.add(QString("FunctionDeclaration"), FunctionDeclarationTest);
    suite.add(QString("FunctionCall"), FunctionCall);
    suite.add(QString("Fib20"), Fib20);

    return suite;
}
