#include "BasePrinter.h"

namespace Caracal
{
    BasePrinter::BasePrinter(i32 indentation)
        : m_indentation{ indentation }
        , m_indentationLevel{ 0 }
        , m_output{ QString() }
        , m_stream{ &m_output }
    {
    }
}
