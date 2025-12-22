#pragma once
//
//#include <Caracal/API.h>
//#include <Caracal/VirtualMachine/ByteCode.h>
//#include <Caracal/Defines.h>
//
//#include <QTextStream>
//
//namespace Caracal
//{
//    class CARACAL_API ByteCodeDisassembler
//    {
//    public:
//        ByteCodeDisassembler(ByteCode& byteCode);
//
//        QString PrettyPrint();
//
//    private:
//        void PrettyPrintOp();
//
//        void PrettyPrintLoadBool();
//        void PrettyPrintNotBool();
//        void PrettyPrintEqualBool();
//        void PrettyPrintNotEqualBool();
//        void PrettyPrintLoadInt32();
//        void PrettyPrintAddInt32();
//        void PrettyPrintSubtractInt32();
//        void PrettyPrintMultiplyInt32();
//        void PrettyPrintDivideInt32();
//        void PrettyPrintNegateInt32();
//        void PrettyPrintEqualInt32();
//        void PrettyPrintNotEqualInt32();
//        void PrettyPrintGreaterInt32();
//        void PrettyPrintGreaterOrEqualInt32();
//        void PrettyPrintLessInt32();
//        void PrettyPrintLessOrEqualInt32();
//        void PrettyPrintFunctionCall();
//        void PrettyPrintJump();
//        void PrettyPrintJumpIfTrue();
//        void PrettyPrintJumpIfFalse();
//        void PrettyPrintMove();
//        void PrettyPrintPrintBool();
//        void PrettyPrintPrintInt32();
//        void PrettyPrintPrintNewLine();
//        void PrettyPrintHalt();
//
//        [[nodiscard]] QTextStream& stream() noexcept { return m_stream; }
//        [[nodiscard]] QByteArray toUtf8() const noexcept { return m_output.toUtf8(); }
//        [[nodiscard]] QString NewLine() const noexcept { return QString("\n"); }
//
//        ByteCode& m_byteCode;
//        u16 m_ip;
//        QString m_output;
//        QTextStream m_stream;
//    };
//}
