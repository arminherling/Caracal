#include "StringBuilder.h"

namespace Caracal
{
    StringBuilder::StringBuilder(i32 indentation)
        : m_indentation{ indentation }
        , m_indentationLevel{ 0 }
        , m_stream{ }
        , m_isEmpty{ true }
    {
    }

    StringBuilder& StringBuilder::append(const std::string_view& text)
    {
        m_stream << text;
        m_isEmpty = false;
        return *this;
    }

    StringBuilder& StringBuilder::appendLine(const std::string_view& text)
    {
        m_stream << text << m_newLine;
        m_isEmpty = false;
        return *this;
    }

    StringBuilder& StringBuilder::appendIndented(const std::string_view& text)
    {
        m_stream << indentation() << text;
        m_isEmpty = false;
        return *this;
    }

    StringBuilder& StringBuilder::appendIndentedLine(const std::string_view& text)
    {
        m_stream << indentation() << text << m_newLine;
        m_isEmpty = false;
        return *this;
    }

    void StringBuilder::clear() noexcept
    {
        m_stream.clear();
        m_indentationLevel = 0;
        m_isEmpty = true;
    }
}
