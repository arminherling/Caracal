#include <Caracal/IR/Module.h>

namespace Caracal
{
    const ExternFunction* Module::tryGetExternFunction(FunctionId id) const noexcept
    {
        const auto result = m_externFunctionIndices.find(id);
        if (result == m_externFunctionIndices.end())
            return nullptr;

        return &m_externFunctions[result->second];
    }

    Function* Module::tryGetFunction(FunctionId id) noexcept
    {
        const auto result = m_functionIndices.find(id);
        if (result == m_functionIndices.end())
            return nullptr;

        return &m_functions[result->second];
    }

    const std::string* Module::tryGetFunctionName(FunctionId id) const noexcept
    {
        if (const auto* externFunction = tryGetExternFunction(id))
            return &externFunction->name();

        const auto result = m_functionIndices.find(id);
        if (result != m_functionIndices.end())
            return &m_functions[result->second].name();

        return nullptr;
    }

    void Module::addExternFunction(ExternFunction function)
    {
        m_externFunctionIndices.emplace(function.id(), m_externFunctions.size());
        m_externFunctions.push_back(std::move(function));
    }

    void Module::addFunction(Function function)
    {
        m_functionIndices.emplace(function.id(), m_functions.size());
        m_functions.push_back(std::move(function));
    }
}
