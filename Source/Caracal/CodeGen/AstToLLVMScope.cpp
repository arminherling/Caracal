#include "AstToLLVMScope.h"

namespace Caracal
{
    AstToLLVMScope::AstToLLVMScope(AstToLLVMScope* parent)
        : m_parent{ parent }
    {
    }

    bool AstToLLVMScope::hasVariableBinding(std::string_view identifier) const noexcept
    {
        if (m_variableBindings.find(identifier) != m_variableBindings.end())
            return true;
        else if (m_parent != nullptr)
            return m_parent->hasVariableBinding(identifier);
        else
            return false;
    }

    void AstToLLVMScope::addVariableBinding(std::string_view identifier, llvm::Value* value)
    {
        m_variableBindings.try_emplace(identifier, value);
    }

    llvm::Value* AstToLLVMScope::getVariableBinding(std::string_view identifier) const noexcept
    {
        if (auto search = m_variableBindings.find(identifier); search != m_variableBindings.end())
            return search->second;
        else if (m_parent != nullptr)
            return m_parent->getVariableBinding(identifier);
        else
            return nullptr;
    }
}
