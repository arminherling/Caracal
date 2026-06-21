#pragma once

#include <string_view>
#include <unordered_map>
#include <optional>

namespace llvm {
    class Value;
}

namespace Caracal
{
    class AstToLLVMScope
    {
    public:
        AstToLLVMScope(AstToLLVMScope* parent);

        bool hasVariableBinding(std::string_view identifier) const noexcept;
        void addVariableBinding(std::string_view identifier, llvm::Value* value);
        [[nodiscard]] llvm::Value* getVariableBinding(std::string_view identifier) const noexcept;

    private:
        AstToLLVMScope* m_parent;
        std::unordered_map<std::string_view, llvm::Value*> m_variableBindings;
    };
}
