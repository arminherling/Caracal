#include <Caracal/IR/Function.h>

namespace Caracal
{
    Function::Function(std::string name, Type returnType)
        : m_name{ std::move(name) }
        , m_returnType{ returnType }
    {
    }

    void Function::addParameterType(Type type)
    {
        m_parameterTypes.push_back(type);
    }

    void Function::addBlock(BasicBlock block)
    {
        m_blocks.push_back(std::move(block));
    }
}
