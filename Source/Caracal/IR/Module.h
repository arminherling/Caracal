#pragma once

#include <Caracal/IR/ConstructedGlobalDeclaration.h>
#include <Caracal/IR/EnumDeclaration.h>
#include <Caracal/IR/ExternFunction.h>
#include <Caracal/IR/Function.h>
#include <Caracal/IR/GlobalConstantDeclaration.h>
#include <Caracal/IR/GlobalReferenceDeclaration.h>
#include <Caracal/IR/TypeDeclaration.h>

#include <optional>
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
        [[nodiscard]] const std::vector<GlobalReferenceDeclaration>& globalReferences() const noexcept { return m_globalReferences; }
        [[nodiscard]] const std::vector<ConstructedGlobalDeclaration>& constructedGlobals() const noexcept { return m_constructedGlobals; }
        [[nodiscard]] const Function* tryGetGlobalInit() const noexcept;
        [[nodiscard]] const std::vector<TypeDeclaration>& types() const noexcept { return m_types; }
        [[nodiscard]] const EnumDeclaration* tryGetEnum(Type type) const noexcept;
        [[nodiscard]] const ExternFunction* tryGetExternFunction(FunctionId id) const noexcept;
        [[nodiscard]] const Function* tryGetFunction(FunctionId id) const noexcept;
        [[nodiscard]] const std::string* tryGetFunctionName(FunctionId id) const noexcept;
        [[nodiscard]] const std::string* tryGetTypeName(Type type) const noexcept;

        void addEnum(EnumDeclaration enumDeclaration);
        void addExternFunction(ExternFunction function);
        Function* addFunction(Function function);
        void addGlobalConstant(GlobalConstantDeclaration globalDeclaration);
        void addGlobalReference(GlobalReferenceDeclaration globalDeclaration);
        void addConstructedGlobal(ConstructedGlobalDeclaration globalDeclaration);
        void setGlobalInit(Function function);
        void addType(TypeDeclaration typeDeclaration);

    private:
        std::vector<EnumDeclaration> m_enums;
        std::vector<ExternFunction> m_externFunctions;
        std::vector<Function> m_functions;
        std::vector<GlobalConstantDeclaration> m_globalConstants;
        std::vector<GlobalReferenceDeclaration> m_globalReferences;
        std::vector<ConstructedGlobalDeclaration> m_constructedGlobals;
        std::optional<Function> m_globalInit;
        std::vector<TypeDeclaration> m_types;
        std::unordered_map<EnumId, size_t> m_enumIndices;
        std::unordered_map<FunctionId, size_t> m_externFunctionIndices;
        std::unordered_map<FunctionId, size_t> m_functionIndices;
        std::unordered_map<i32, size_t> m_typeIndices;
    };
}
