#pragma once

#include <Caracal/Defines.h>
#include <Caracal/API.h>
#include <Caracal/Semantic/Type.h>
#include <Caracal/Semantic/ConstantDefinition.h>
#include <Caracal/Semantic/EnumDefinition.h>
#include <Caracal/Semantic/TypeDefinition.h>
#include <Caracal/Semantic/FunctionDefinition.h>
#include <Caracal/Syntax/EnumDefinitionStatement.h>
#include <Caracal/Syntax/TypeDefinitionStatement.h>
#include <Caracal/Syntax/FunctionDefinitionStatement.h>
#include <Caracal/Syntax/MethodDefinitionStatement.h>
#include <Caracal/Semantic/Parameter.h>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace Caracal
{
    class CARACAL_API SemanticContext
    {
    public:
        SemanticContext() = default;

        [[nodiscard]] static SemanticContext WithBuiltins() noexcept;

        [[nodiscard]] Type tryGetFunctionTypeByName(std::string_view typeName) const noexcept;
        [[nodiscard]] EnumDefinition& getEnumDefinition(Type type) noexcept;
        [[nodiscard]] TypeDefinition& getTypeDefinition(Type type) noexcept;
        [[nodiscard]] FunctionDefinition& getFunctionDefinition(Type type) noexcept;
        [[nodiscard]] Type tryGetTypeByName(std::string_view name) const noexcept;
        [[nodiscard]] std::string_view getNameByType(Type type) noexcept;

        [[nodiscard]] EnumDefinition& createEnum(
            std::string_view name,
            const EnumDefinitionStatement* statement) noexcept;
        [[nodiscard]] TypeDefinition& createType(
            std::string_view name,
            const TypeDefinitionStatement* statement) noexcept;
        [[nodiscard]] FunctionDefinition& createFunction(
            std::string_view name,
            const std::vector<Parameter>& parameters,
            const std::vector<Type>& returnTypes,
            const FunctionDefinitionStatement* statement) noexcept;
        [[nodiscard]] FunctionDefinition& createMethod(
            TypeDefinition& typeDefinition,
            MethodModifier modifier,
            const std::string& methodName,
            const std::vector<Parameter>& parameters,
            const std::vector<Type>& returnTypes,
            const MethodDefinitionStatement* statement) noexcept;
        [[nodiscard]] FunctionDefinition& createConstructor(
            TypeDefinition& typeDefinition,
            const std::vector<Parameter>& parameters) noexcept;
        [[nodiscard]] ConstantDefinition& createConstant(
            std::string_view name,
            Expression* expression) noexcept;
        void createBuiltinType(Type type, std::string_view name, bool addVariants = false);

        [[nodiscard]] const std::vector<TypeDefinition>& typeDefinitions() const noexcept { return m_typeDefinitions; }
        [[nodiscard]] const std::vector<ConstantDefinition>& constantDefinitions() const noexcept { return m_constantDefinitions; }
        [[nodiscard]] const std::vector<EnumDefinition>& enumDefinitions() const noexcept { return m_enumDefinitions; }
        [[nodiscard]] const std::vector<FunctionDefinition>& functionDefinitions() const noexcept { return m_functionDefinitions; }

    private:
        std::vector<TypeDefinition> m_typeDefinitions;
        std::vector<EnumDefinition> m_enumDefinitions;
        std::vector<FunctionDefinition> m_functionDefinitions;
        std::vector<ConstantDefinition> m_constantDefinitions;
        std::unordered_map<i32, size_t> m_typeDefinitionIndexById;
        std::unordered_map<i32, size_t> m_enumDefinitionIndexById;
        std::unordered_map<i32, size_t> m_functionDefinitionIndexById;
        std::unordered_map<std::string, size_t> m_constantDefinitionIndexByName;
        std::unordered_map<i32, std::string> m_typeNames;
        std::unordered_map<std::string, Type> m_nameToTypes;
        i32 m_nextId = 0;
    };
}
