#include <CodeGen/CppCodeGenerator.h>

namespace Caracal
{
    CppCodeGenerator::CppCodeGenerator(ParseTree& parseTree, i32 indentation)
        : BasePrinter(indentation)
        , m_parseTree{ parseTree }
    {
    }

    QString CppCodeGenerator::generate()
    {
        return toUtf8();
    }

    QString generateCpp(ParseTree& parseTree) noexcept
    {
        CppCodeGenerator generator{ parseTree };
        return generator.generate();
    }
}
