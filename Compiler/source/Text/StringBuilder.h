#pragma once

#include <Compiler/API.h>
#include <Defines.h>
#include <sstream>
#include <string>

namespace Caracal
{
    class COMPILER_API StringBuilder
    {
    public:
        explicit StringBuilder(i32 indentation = 4);

        CARACAL_DELETE_COPY_DEFAULT_MOVE(StringBuilder)

        [[nodiscard]] std::string toString() const noexcept { return m_stream.str(); }

        StringBuilder& append(const std::string_view& text);
        StringBuilder& appendLine(const std::string_view& text);
        StringBuilder& appendIndented(const std::string_view& text);
        StringBuilder& appendIndentedLine(const std::string_view& text);

        void clear() noexcept;
        void setNewLine(const std::string_view& newLine) noexcept { m_newLine = newLine; }
        void pushIndentation() noexcept { m_indentationLevel++; }
        void popIndentation() noexcept { m_indentationLevel--; }

    private:
        [[nodiscard]] std::ostringstream& stream() noexcept { return m_stream; }
        [[nodiscard]] std::string indentation() const noexcept { return std::string(m_indentation * m_indentationLevel, ' '); }

        i32 m_indentation;
        i32 m_indentationLevel;
        std::ostringstream m_stream;
        std::string m_newLine{ "\n" };
    };
}
