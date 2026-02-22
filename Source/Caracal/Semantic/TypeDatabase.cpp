#include "TypeDatabase.h"
#include "TypeDatabase.h"
#include "TypeDatabase.h"
#include "TypeDatabase.h"

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

    TypeDefinition& TypeDatabase::getTypeDefinition(Type type) noexcept
    {
        static auto invalidType = TypeDefinition{ Type::Undefined(), std::string("???") };
        
        auto id = type.id();
        if (m_typeDefinitions.contains(id))
            return m_typeDefinitions.at(id);
        else
            return invalidType;
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

    MethodDefinition& TypeDatabase::getMethodDefinition(Type type) noexcept
    {
        static auto invalidMethod = MethodDefinition{ Type::Undefined(), Type::Undefined(), std::string("???"), MethodModifier::None };
        
        auto id = type.id();
        if (m_methodDefinitions.contains(id))
            return m_methodDefinitions.at(id);
        else
            return invalidMethod;
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

    TypeDefinition& TypeDatabase::createType(std::string_view name) noexcept
    {
        auto typeName = std::string(name);
        auto typeId = m_nextId++;
        auto newType = Type{ typeId, TypeKind::Type };
        m_names.try_emplace(typeName, newType);
        m_typeDefinitions.try_emplace(typeId, TypeDefinition{ newType, typeName });

        return m_typeDefinitions.at(typeId);
    }

    MethodDefinition& TypeDatabase::createMethod(
        TypeDefinition& typeDefinition, 
        MethodModifier modifier, 
        const std::string& methodName, 
        const std::vector<Parameter>& parameters, 
        const std::vector<Type>& returnTypes) noexcept
    {
        auto parentType = typeDefinition.type();
        auto methodId = m_nextId++;
        auto methodType = Type{ methodId, TypeKind::Method };
        m_methodDefinitions.try_emplace(methodId, MethodDefinition{ parentType, methodType, methodName, modifier, parameters, returnTypes });
        typeDefinition.addMethod(methodType, methodName);

        return m_methodDefinitions.at(methodId);
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
