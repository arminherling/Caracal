#include <Caracal/IR/Module.h>

namespace Caracal
{
    void Module::addFunction(Function function)
    {
        m_functions.push_back(std::move(function));
    }
}
