#pragma once

#include <Caracal/Defines.h>
#include <Caracal/API.h>
#include <Caracal/Semantic/Type.h>
#include <Caracal/Semantic/EnumDefinition.h>
#include <Caracal/Semantic/TypeDefinition.h>
#include <Caracal/Semantic/FunctionDefinition.h>
#include <Caracal/Semantic/MethodDefinition.h>
#include <Caracal/Semantic/Parameter.h>
#include <string_view>
#include <unordered_map>

namespace Caracal
{
    class CARACAL_API TypeDatabase
    {
    public:
        TypeDatabase();

        [[nodiscard]] static Type TryFindBuiltin(std::string_view typeName) noexcept;
        [[nodiscard]] static std::string_view TryFindName(Type type) noexcept;

        [[nodiscard]] Type tryGetFunctionTypeByName(std::string_view typeName) const noexcept;
        [[nodiscard]] EnumDefinition& getEnumDefinition(Type type) noexcept;
        [[nodiscard]] TypeDefinition& getTypeDefinition(Type type) noexcept;
        [[nodiscard]] FunctionDefinition& getFunctionDefinition(Type type) noexcept;
        [[nodiscard]] MethodDefinition& getMethodDefinition(Type type) noexcept;
        [[nodiscard]] std::optional<Type> tryGetTypeByName(std::string_view name) const noexcept;

        [[nodiscard]] EnumDefinition& createEnum(std::string_view name) noexcept;
        [[nodiscard]] TypeDefinition& createType(std::string_view name) noexcept;
        [[nodiscard]] MethodDefinition& createMethod(
            TypeDefinition& typeDefinition,
            MethodModifier modifier,
            const std::string& methodName,
            const std::vector<Parameter>& parameters,
            const std::vector<Type>& returnTypes) noexcept;

            [[nodiscard]] FunctionDefinition& createFunction(
            std::string_view name, 
            const std::vector<Parameter>& parameters, 
            const std::vector<Type>& returnTypes) noexcept;

    private:
        std::unordered_map<std::string, Type> m_names;
        std::unordered_map<i32, EnumDefinition> m_enumDefinitions;
        std::unordered_map<i32, FunctionDefinition> m_functionDefinitions;
        std::unordered_map<i32, TypeDefinition> m_typeDefinitions;
        std::unordered_map<i32, MethodDefinition> m_methodDefinitions;
        i32 m_nextId = 2000;
    };
}
