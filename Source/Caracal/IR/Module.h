#pragma once

#include <Caracal/IR/EnumDeclaration.h>
#include <Caracal/IR/ExternFunction.h>
#include <Caracal/IR/Function.h>

#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace Caracal
{
    class Module
    {
    public:
        [[nodiscard]] const std::vector<EnumDeclaration>& enums() const noexcept { return m_enums; }
        [[nodiscard]] const std::vector<ExternFunction>& externFunctions() const noexcept { return m_externFunctions; }
        [[nodiscard]] const std::vector<Function>& functions() const noexcept { return m_functions; }
        [[nodiscard]] const EnumDeclaration* tryGetEnum(Type type) const noexcept;
        [[nodiscard]] const ExternFunction* tryGetExternFunction(FunctionId id) const noexcept;
        [[nodiscard]] Function* tryGetFunction(FunctionId id) noexcept;
        [[nodiscard]] const std::string* tryGetFunctionName(FunctionId id) const noexcept;

        void addEnum(EnumDeclaration enumDeclaration);
        void addExternFunction(ExternFunction function);
        void addFunction(Function function);

    private:
        std::vector<EnumDeclaration> m_enums;
        std::vector<ExternFunction> m_externFunctions;
        std::vector<Function> m_functions;
        std::unordered_map<EnumId, size_t> m_enumIndices;
        std::unordered_map<FunctionId, size_t> m_externFunctionIndices;
        std::unordered_map<FunctionId, size_t> m_functionIndices;
    };
}
