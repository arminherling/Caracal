#include "TypeDatabase.h"
//
//TypeDatabase::TypeDatabase()
//    : m_nextId{ 100 }
//{
//    addBuiltinType(Type::Discard(), QString("_"));
//    addBuiltinType(Type::Void(), QString("void"));
//    addBuiltinTypesWithVariation(Type::Bool(), QString("bool"));
//    addBuiltinTypesWithVariation(Type::U8(), QString("u8"));
//    addBuiltinTypesWithVariation(Type::I32(), QString("i32"));
//}
//
//Type TypeDatabase::getTypeByName(QStringView typeName) const noexcept
//{
//    auto name = typeName.toString();
//    if (m_names.contains(name))
//        return m_names.at(name);
//    else
//        return Type::Undefined();
//}
//TypeDefinition& TypeDatabase::getTypeDefinition(Type type) noexcept
//{
//    static auto invalidType = TypeDefinition{ Type::Undefined(), QString("???") };
//
//    auto id = type.id();
//    if (m_typeDefinitions.contains(id))
//        return m_typeDefinitions.at(id);
//    else
//        return invalidType;
//}
//Type TypeDatabase::createType(QStringView name, TypeKind kind) noexcept
//{
//    auto typeName = name.toString();
//    auto type = Type{ m_nextId++, kind };
//    m_names.emplace(typeName, type);
//    m_typeDefinitions.emplace(type.id(), TypeDefinition{type, typeName});
//    return type;
//}

//
//void TypeDatabase::addBuiltinTypesWithVariation(Type type, const QString& name) noexcept
//{
//    addBuiltinType(type, name);
//
//    auto refTypeName = QString("ref %1").arg(name);
//    auto refType = Type(type.id() + 1, type.kind());
//    addBuiltinType(refType, refTypeName);
//
//    //auto nullableTypeName = QString("%1?").arg(name);
//    //auto nullableType = Type(type.id() + 2, type.kind());
//    //addBuiltinType(nullableType, nullableTypeName);
//
//    //auto nullableRefTypeName = QString("ref %1?").arg(name);
//    //auto nullableRefType = Type(type.id() + 3, type.kind());
//    //addBuiltinType(nullableRefType, nullableRefTypeName);
//}
//
//void TypeDatabase::addBuiltinType(Type type, const QString& name) noexcept
//{
//    m_names.emplace(name, type);
//    m_typeDefinitions.emplace(type.id(), TypeDefinition{ type, name });
//}

namespace Caracal
{
    [[nodiscard]] static auto InitializeBuiltinTypes() noexcept
    {
        return std::unordered_map<std::string_view, Type>{
            { std::string_view("..."), Type::CVariadic() },
            { std::string_view("bool"), Type::Bool()},
            { std::string_view("i32"), Type::I32() },
            { std::string_view("f32"), Type::F32() },
            { std::string_view("string"), Type::String() },
        };
    }

    [[nodiscard]] static auto InitializeTypeToName() noexcept
    {
        return std::unordered_map<Type, std::string_view>{
            { Type::CVariadic(), std::string_view("C Variadic") },
            { Type::Function(), std::string_view("function") },
            { Type::Undefined(), std::string_view("undefined") },
            { Type::Void(), std::string_view("void") },
            { Type::Bool(), std::string_view("bool") },
            { Type::U8(), std::string_view("u8") },
            { Type::I32(), std::string_view("i32") },
            { Type::F32(), std::string_view("f32") },
            { Type::String(), std::string_view("string") },
        };
    }

    TypeDatabase::TypeDatabase()
    {
        // TODO remove this once we got a prelude
        m_functionDefinitions.try_emplace(1000, FunctionDefinition{ Type{ 1000, TypeKind::Function }, "print", false, {Parameter{"msg", Type::String()}} });
    }

    Type TypeDatabase::TryFindBuiltin(std::string_view typeName) noexcept
    {
        static const auto typeNames = InitializeBuiltinTypes();
        if (const auto result = typeNames.find(typeName); result != typeNames.end())
            return result->second;

        return Type::Undefined();
    }

    std::string_view TypeDatabase::TryFindName(Type type) noexcept
    {
        static const auto typeNames = InitializeTypeToName();
        if (const auto result = typeNames.find(type); result != typeNames.end())
            return result->second;

        return std::string_view("???");
    }

    Type TypeDatabase::tryGetFunctionTypeByName(std::string_view typeName) const noexcept
    {
        for (const auto& [id, functionDef] : m_functionDefinitions)
        {
            if (functionDef.name() == typeName)
                return functionDef.type();
        }

        return Type::Undefined();
    }

    EnumDefinition& TypeDatabase::getEnumDefinition(Type type) noexcept
    {
        static auto invalidEnum = EnumDefinition{ Type::Undefined(), std::string("???") };
    
        auto id = type.id();
        if (m_enumDefinitions.contains(id))
            return m_enumDefinitions.at(id);
        else
            return invalidEnum;
    }
    
    FunctionDefinition& TypeDatabase::getFunctionDefinition(Type type) noexcept
    {
        static auto invalidFunction = FunctionDefinition{ Type::Undefined(), std::string("???"), false };

        auto id = type.id();
        if (m_functionDefinitions.contains(id))
            return m_functionDefinitions.at(id);
        else
            return invalidFunction;
    }

    std::optional<Type> TypeDatabase::tryGetTypeByName(std::string_view name) const noexcept
    {
        if (const auto result = m_names.find(std::string(name)); result != m_names.end())
            return result->second;

        return std::nullopt;
    }

    EnumDefinition& TypeDatabase::createEnum(std::string_view name) noexcept
    {
        auto enumName = std::string(name);
        auto enumId = m_nextId++;
        auto enumType = Type{ enumId, TypeKind::Enum };
        m_names.try_emplace(enumName, enumType);
        m_enumDefinitions.try_emplace(enumId, EnumDefinition{ enumType, enumName });

        return m_enumDefinitions.at(enumId);
    }
    
    FunctionDefinition& TypeDatabase::createFunction(
        std::string_view name,
        const std::vector<Parameter>& parameters,
        const std::vector<Type>& returnTypes) noexcept
    {
        auto functionName = std::string(name);
        auto functionId = m_nextId++;
        auto functionType = Type{ functionId, TypeKind::Function };
        auto isVariadic = !parameters.empty() && parameters.back().type() == Type::CVariadic();
        m_functionDefinitions.try_emplace(functionId, FunctionDefinition{ functionType, functionName, isVariadic, parameters, returnTypes });

        return m_functionDefinitions.at(functionId);
    }
}
