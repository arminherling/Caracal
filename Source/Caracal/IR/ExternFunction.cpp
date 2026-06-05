#include <Caracal/IR/ExternFunction.h>

namespace Caracal
{
    ExternFunction::ExternFunction(FunctionId id, std::string name, const std::vector<IRParameter>& parameters, Type returnType)
        : m_id{ id }
        , m_name{ std::move(name) }
        , m_parameters{ parameters }
        , m_returnType{ returnType }
    {
    }

    void ExternFunction::addParameter(IRParameter parameter)
    {
        m_parameters.push_back(std::move(parameter));
    }
}
