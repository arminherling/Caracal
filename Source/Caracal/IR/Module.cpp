#include <Caracal/IR/Module.h>

namespace Caracal
{
    void Module::addExternFunction(ExternFunction function)
    {
        m_externFunctions.push_back(std::move(function));
    }

    void Module::addFunction(Function function)
    {
        m_functions.push_back(std::move(function));
    }
}
