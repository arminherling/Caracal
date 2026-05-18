#pragma once

#include <Caracal/IR/ExternFunction.h>
#include <Caracal/IR/Function.h>

#include <utility>
#include <vector>

namespace Caracal
{
    class Module
    {
    public:
        [[nodiscard]] const std::vector<ExternFunction>& externFunctions() const noexcept { return m_externFunctions; }
        [[nodiscard]] const std::vector<Function>& functions() const noexcept { return m_functions; }

        void addExternFunction(ExternFunction function);
        void addFunction(Function function);

    private:
        std::vector<ExternFunction> m_externFunctions;
        std::vector<Function> m_functions;
    };
}
