#pragma once

#include <Caracal/Defines.h>
#include <Caracal/API.h>
#include <Caracal/Semantic/Type.h>
#include <Caracal/Semantic/EnumDefinition.h>
#include <Caracal/Semantic/TypeDefinition.h>
#include <Caracal/Semantic/FunctionDefinition.h>
#include <Caracal/syntax/MethodDefinitionStatement.h>
#include <Caracal/Semantic/Parameter.h>
#include <string_view>
#include <unordered_map>

namespace Caracal
{
    class CARACAL_API Module
    {
    public:
        Module() = default;

        [[nodiscard]] static Module WithBuiltins() noexcept;

        //[[nodiscard]] static Type TryFindBuiltin(std::string_view typeName) noexcept;

        [[nodiscard]] Type tryGetFunctionTypeByName(std::string_view typeName) const noexcept;
        [[nodiscard]] EnumDefinition& getEnumDefinition(Type type) noexcept;
        [[nodiscard]] TypeDefinition& getTypeDefinition(Type type) noexcept;
        [[nodiscard]] FunctionDefinition& getFunctionDefinition(Type type) noexcept;
        [[nodiscard]] Type tryGetTypeByName(std::string_view name) const noexcept;
        [[nodiscard]] std::string_view getNameByType(Type type) noexcept;

        [[nodiscard]] EnumDefinition& createEnum(std::string_view name) noexcept;
        [[nodiscard]] TypeDefinition& createType(std::string_view name) noexcept;
        [[nodiscard]] FunctionDefinition& createFunction(
            std::string_view name, 
            const std::vector<Parameter>& parameters, 
            const std::vector<Type>& returnTypes) noexcept;
        [[nodiscard]] FunctionDefinition& createMethod(
            TypeDefinition& typeDefinition,
            MethodModifier modifier,
            const std::string& methodName,
            const std::vector<Parameter>& parameters,
            const std::vector<Type>& returnTypes) noexcept;
        void createBuiltinType(Type type, std::string_view name, bool addVariants = false);

    private:
        std::unordered_map<i32, TypeDefinition> m_typeDefinitions;
        std::unordered_map<i32, EnumDefinition> m_enumDefinitions;
        std::unordered_map<i32, FunctionDefinition> m_functionDefinitions;
        std::unordered_map<i32, std::string> m_typeNames;
        std::unordered_map<std::string, Type> m_nameToTypes;
        i32 m_nextId = 0;
    };
}
