#include <Caracal/IR/ExternFunction.h>

namespace Caracal
{
    ExternFunction::ExternFunction(std::string name, const std::vector<Type>& parameterTypes, Type returnType)
        : m_name{ std::move(name) }
        , m_parameterTypes{ parameterTypes }
        , m_returnType{ returnType }
    {
    }

    void ExternFunction::addParameterType(Type type)
    {
        m_parameterTypes.push_back(type);
    }
}
