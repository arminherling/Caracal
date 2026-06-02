#include <Caracal/IR/Module.h>

namespace Caracal
{
    const EnumDeclaration* Module::tryGetEnum(Type type) const noexcept
    {
        const auto result = m_enumIndices.find(type.id());
        if (result == m_enumIndices.end())
            return nullptr;

        return &m_enums[result->second];
    }

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

    void Module::addEnum(EnumDeclaration enumDeclaration)
    {
        const auto enumId = enumDeclaration.type().id();
        m_enumIndices.emplace(enumId, m_enums.size());
        m_enums.push_back(std::move(enumDeclaration));
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
