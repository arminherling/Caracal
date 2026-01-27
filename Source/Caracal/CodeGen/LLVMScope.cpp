#include "LLVMScope.h"

namespace Caracal
{
    LLVMScope::LLVMScope(LLVMScope* parent)
        : m_parent{ parent }
    {
    }

    bool LLVMScope::hasVariableBinding(std::string_view identifier) const noexcept
    {
        if (m_variableBindings.find(identifier) != m_variableBindings.end())
            return true;
        else if (m_parent != nullptr)
            return m_parent->hasVariableBinding(identifier);
        else
            return false;
    }

    void LLVMScope::addVariableBinding(std::string_view identifier, llvm::Value* value)
    {
        m_variableBindings.try_emplace(identifier, value);
    }

    llvm::Value* LLVMScope::getVariableBinding(std::string_view identifier) const noexcept
    {
        if (auto search = m_variableBindings.find(identifier); search != m_variableBindings.end())
            return search->second;
        else if (m_parent != nullptr)
            return m_parent->getVariableBinding(identifier);
        else
            return nullptr;
    }
}
