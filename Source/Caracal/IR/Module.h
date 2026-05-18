#pragma once

#include <Caracal/IR/Function.h>

#include <utility>
#include <vector>

namespace Caracal
{
    class Module
    {
    public:
        [[nodiscard]] const std::vector<Function>& functions() const noexcept { return m_functions; }
        void addFunction(Function function);

    private:
        std::vector<Function> m_functions;
    };
}
