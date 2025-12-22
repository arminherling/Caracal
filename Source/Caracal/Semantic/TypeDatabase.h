#pragma once

#include <Caracal/Defines.h>
#include <Caracal/API.h>
//#include <Semantic/EnumDefinition.h>
#include <Caracal/Semantic/Type.h>
//#include <Semantic/TypeDefinition.h>
//#include <Semantic/FunctionDefinition.h>
#include <string_view>
#include <unordered_map>

namespace Caracal
{
    class CARACAL_API TypeDatabase
    {
    public:
        [[nodiscard]] static Type TryFindBuiltin(std::string_view typeName) noexcept;
        [[nodiscard]] static std::string_view TryFindName(Type type) noexcept;

    private:
        TypeDatabase();

    //    [[nodiscard]] Type getTypeByName(QStringView typeName) const noexcept;
    //    [[nodiscard]] EnumDefinition& getEnumDefinition(Type type) noexcept;
    //    [[nodiscard]] TypeDefinition& getTypeDefinition(Type type) noexcept;
    //    [[nodiscard]] FunctionDefinition& getFunctionDefinition(Type type) noexcept;

    //    [[nodiscard]] Type createEnum(QStringView name) noexcept;
    //    [[nodiscard]] Type createType(QStringView name, TypeKind kind) noexcept;
    //    [[nodiscard]] Type createFunction(QStringView name) noexcept;

    //private:
    //    std::unordered_map<QString, Type> m_names;
    //    std::unordered_map<i32, EnumDefinition> m_enumDefinitions;
    //    std::unordered_map<i32, TypeDefinition> m_typeDefinitions;
    //    std::unordered_map<i32, FunctionDefinition> m_functionDefinitions;
    //    i32 m_nextId;

    //    void addBuiltinTypesWithVariation(Type type, const QString& name) noexcept;
    //    void addBuiltinType(Type type, const QString& name) noexcept;
    };
}
