#pragma once

#include <Compiler/API.h>
#include <Defines.h>
#include <QTextStream>
#include <QString>

namespace Caracal 
{
    class COMPILER_API BasePrinter
    {
    public:
        BasePrinter(i32 indentation = 4);

    protected:
        void pushIndentation() noexcept { m_indentationLevel++; }
        void popIndentation() noexcept { m_indentationLevel--; }
        [[nodiscard]] QTextStream& stream() noexcept { return m_stream; }
        [[nodiscard]] QByteArray toUtf8() const noexcept { return m_output.toUtf8(); }

        [[nodiscard]] QString indentation() const noexcept { return QString(" ").repeated(m_indentation * m_indentationLevel); }
        [[nodiscard]] QString newLine() const noexcept { return QString("\r\n"); }

        i32 m_indentation;
        i32 m_indentationLevel;
        QString m_output;
        QTextStream m_stream;
    };
}
