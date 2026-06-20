#pragma once

#include <Caracal/IR/EnumDeclaration.h>
#include <Caracal/IR/ExternFunction.h>
#include <Caracal/IR/Function.h>
#include <Caracal/IR/GlobalConstantDeclaration.h>
#include <Caracal/IR/TypeDeclaration.h>

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
        [[nodiscard]] const std::vector<GlobalConstantDeclaration>& globalConstants() const noexcept { return m_globalConstants; }
        [[nodiscard]] const std::vector<TypeDeclaration>& types() const noexcept { return m_types; }
        [[nodiscard]] const EnumDeclaration* tryGetEnum(Type type) const noexcept;
        [[nodiscard]] const ExternFunction* tryGetExternFunction(FunctionId id) const noexcept;
        [[nodiscard]] Function* tryGetFunction(FunctionId id) noexcept;
        [[nodiscard]] const std::string* tryGetFunctionName(FunctionId id) const noexcept;
        [[nodiscard]] const std::string* tryGetTypeName(Type type) const noexcept;

        void addEnum(EnumDeclaration enumDeclaration);
        void addExternFunction(ExternFunction function);
        Function* addFunction(Function function);
        void addGlobalConstant(GlobalConstantDeclaration globalDeclaration);
        void addType(TypeDeclaration typeDeclaration);

    private:
        std::vector<EnumDeclaration> m_enums;
        std::vector<ExternFunction> m_externFunctions;
        std::vector<Function> m_functions;
        std::vector<GlobalConstantDeclaration> m_globalConstants;
        std::vector<TypeDeclaration> m_types;
        std::unordered_map<EnumId, size_t> m_enumIndices;
        std::unordered_map<FunctionId, size_t> m_externFunctionIndices;
        std::unordered_map<FunctionId, size_t> m_functionIndices;
        std::unordered_map<i32, size_t> m_typeIndices;
    };
}
