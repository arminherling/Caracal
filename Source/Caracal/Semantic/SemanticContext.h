#pragma once

#include <Caracal/Defines.h>
#include <Caracal/API.h>
#include <Caracal/Constants.h>
#include <Caracal/Semantic/Type.h>
#include <Caracal/Semantic/BuiltinTypeDescription.h>
#include <Caracal/Semantic/TypeCheckerOptions.h>
#include <Caracal/Semantic/ConstantDefinition.h>
#include <Caracal/Semantic/EnumDefinition.h>
#include <Caracal/Semantic/TypeDefinition.h>
#include <Caracal/Semantic/FunctionDefinition.h>
#include <Caracal/Syntax/EnumDefinitionStatement.h>
#include <Caracal/Syntax/TypeDefinitionStatement.h>
#include <Caracal/Syntax/FunctionDefinitionStatement.h>
#include <Caracal/Syntax/MethodDefinitionStatement.h>
#include <Caracal/Syntax/ParseTree.h>
#include <Caracal/Semantic/Parameter.h>
#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace Caracal
{
    class CARACAL_API SemanticContext
    {
    public:
        SemanticContext() = default;

        CARACAL_DELETE_COPY_DEFAULT_MOVE(SemanticContext)

        [[nodiscard]] static SemanticContext WithBuiltins(const std::vector<std::string>& preludeSources, const TypeCheckerOptions& options) noexcept;
        [[nodiscard]] static std::vector<std::string> CollectPreludeSources(const std::filesystem::path& preludeDirectory) noexcept;

        [[nodiscard]] Type tryGetFunctionTypeByName(std::string_view typeName) const noexcept;
        [[nodiscard]] EnumDefinition& getEnumDefinition(Type type) noexcept;
        [[nodiscard]] TypeDefinition& getTypeDefinition(Type type) noexcept;
        [[nodiscard]] const OperatorSignature* tryGetOperatorSignature(Type type, BinaryOperatorKind operation) const noexcept;
        [[nodiscard]] const OperatorSignature* tryGetOperatorSignature(Type type, UnaryOperatorKind operation) const noexcept;
        TypeDefinition& bindBuiltinTypeDefinition(Type type, const TypeDefinitionStatement* statement) noexcept;
        [[nodiscard]] FunctionDefinition& getFunctionDefinition(Type type) noexcept;
        [[nodiscard]] const FunctionDefinition* tryGetFunctionDefinition(Type type) const noexcept;
        [[nodiscard]] Type tryGetTypeByName(std::string_view name) const noexcept;
        [[nodiscard]] std::string_view getNameByType(Type type) const noexcept;

        [[nodiscard]] Type getOrCreateArrayType(TypeKind arrayKind, Type elementType, i32 length) noexcept;
        [[nodiscard]] Type getArrayElementType(Type type) const noexcept;
        [[nodiscard]] i32 getArrayLength(Type type) const noexcept;
        [[nodiscard]] const std::vector<Type>& arrayTypes() const noexcept { return m_arrayTypes; }
        [[nodiscard]] Type tryGetOrCreateArrayIntrinsic(Type arrayType, std::string_view methodName) noexcept;

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
        [[nodiscard]] const ConstantDefinition* tryGetConstantDefinition(std::string_view name) const noexcept;
        [[nodiscard]] ConstantDefinition& createConstant(
            std::string_view name,
            Expression* expression) noexcept;
        [[nodiscard]] ConstantDefinition& createInitConstant(
            std::string_view name,
            Type type) noexcept;
        void addGlobalDiscardExpression(const Expression* expression) noexcept;
        void createBuiltinType(Type type, std::string_view name, bool addVariants = false);
        [[nodiscard]] TypeDefinition& createBuiltinTypeFromDescription(std::string_view name, const BuiltinTypeDescription& description, const TypeDefinitionStatement* statement) noexcept;
        [[nodiscard]] const BuiltinTypeDescription* tryGetBuiltinTypeDescription(Type type) const noexcept;
        [[nodiscard]] const std::unordered_map<i32, BuiltinTypeDescription>& builtinTypeDescriptions() const noexcept { return m_builtinTypeDescriptionsById; }
        void refreshWellKnownTypes() noexcept;
        [[nodiscard]] const WellKnownTypes& wellKnown() const noexcept { return m_wellKnownTypes; }

        [[nodiscard]] const std::vector<TypeDefinition>& typeDefinitions() const noexcept { return m_typeDefinitions; }
        [[nodiscard]] const std::vector<ConstantDefinition>& constantDefinitions() const noexcept { return m_constantDefinitions; }
        [[nodiscard]] const std::vector<const Expression*>& globalDiscardExpressions() const noexcept { return m_globalDiscardExpressions; }
        [[nodiscard]] const std::vector<EnumDefinition>& enumDefinitions() const noexcept { return m_enumDefinitions; }
        [[nodiscard]] const std::vector<FunctionDefinition>& functionDefinitions() const noexcept { return m_functionDefinitions; }

    private:
        struct ArrayTypeInfo
        {
            Type elementType;
            i32 length;
        };

        std::vector<TypeDefinition> m_typeDefinitions;
        std::vector<EnumDefinition> m_enumDefinitions;
        std::vector<FunctionDefinition> m_functionDefinitions;
        std::vector<ConstantDefinition> m_constantDefinitions;
        std::vector<const Expression*> m_globalDiscardExpressions;
        std::unordered_map<i32, size_t> m_typeDefinitionIndexById;
        std::unordered_map<i32, size_t> m_enumDefinitionIndexById;
        std::unordered_map<i32, size_t> m_functionDefinitionIndexById;
        std::unordered_map<std::string, size_t> m_constantDefinitionIndexByName;
        std::unordered_map<i32, std::string> m_typeNames;
        std::unordered_map<std::string, Type> m_nameToTypes;
        std::unordered_map<i32, ArrayTypeInfo> m_arrayTypeInfoById;
        std::vector<Type> m_arrayTypes;
        std::unordered_map<std::string, Type> m_arrayIntrinsicsByFullName;
        std::unordered_map<i32, BuiltinTypeDescription> m_builtinTypeDescriptionsById;
        WellKnownTypes m_wellKnownTypes;
        i32 m_nextId = 0;
        // TODO keep the parsetrees alive here for now
        std::vector<ParseTreeUPtr> m_preludeParseTrees;
    };
}
