#include "Module.h"

namespace Caracal
{
    // value, reference, optional value, optional reference
    constexpr int VariantCount = 4; 
    
    [[nodiscard]] static FunctionType ToFunctionType(MethodModifier modifier)
    {
        switch (modifier)
        {
            case MethodModifier::Public:
                return FunctionType::PublicMethod;
            case MethodModifier::Private:
                return FunctionType::PrivateMethod;
            case MethodModifier::Static:
                return FunctionType::StaticMethod;
            default:
                return FunctionType::None;
        }
    }

    Module Module::WithBuiltins() noexcept
    {
        Module module{};
        module.createBuiltinType(Type::CVariadic(), "...", false);
        module.createBuiltinType(Type::Function(), "function", false);
        module.createBuiltinType(Type::Discard(), "discard", false);
        module.createBuiltinType(Type::Undefined(), "undefined", false);
        module.createBuiltinType(Type::Void(), "void", false);

        // TODO remove this once we got a prelude
        module.createBuiltinType(Type::Bool(), "bool");
        module.createBuiltinType(Type::U8(), "u8");
        module.createBuiltinType(Type::I32(), "i32");
        module.createBuiltinType(Type::F32(), "f32");
        module.createBuiltinType(Type::String(), "string");
        module.m_functionDefinitions.try_emplace(1000, FunctionDefinition{ Type{ 1000, TypeKind::Function }, Type::Undefined(), FunctionType::FreeFunction, "print", "print", false, {Parameter{"msg", Type::String()}} });
        module.m_nextId = 2000;

        return module;
    }

    Type Module::tryGetFunctionTypeByName(std::string_view typeName) const noexcept
    {
        for (const auto& [id, functionDef] : m_functionDefinitions)
        {
            if (functionDef.name() == typeName)
                return functionDef.type();
        }

        return Type::Undefined();
    }

    EnumDefinition& Module::getEnumDefinition(Type type) noexcept
    {
        static auto invalidEnum = EnumDefinition{ Type::Undefined(), std::string("???") };
    
        auto baseType = type.toBaseType();
        auto id = baseType.id();
        if (m_enumDefinitions.contains(id))
            return m_enumDefinitions.at(id);
        else
            return invalidEnum;
    }

    TypeDefinition& Module::getTypeDefinition(Type type) noexcept
    {
        static auto invalidType = TypeDefinition{ Type::Undefined(), std::string("???") };
        
        auto baseType = type.toBaseType();
        auto id = baseType.id();
        if (m_typeDefinitions.contains(id))
            return m_typeDefinitions.at(id);
        else
            return invalidType;
    }
    
    FunctionDefinition& Module::getFunctionDefinition(Type type) noexcept
    {
        static auto invalidFunction = FunctionDefinition{ Type::Undefined(), Type::Undefined(), FunctionType::None, std::string("???"), std::string("???"), false };

        auto id = type.id();
        if (m_functionDefinitions.contains(id))
            return m_functionDefinitions.at(id);
        else
            return invalidFunction;
    }

    Type Module::tryGetTypeByName(std::string_view name) const noexcept
    {
        if (const auto result = m_nameToTypes.find(std::string(name)); result != m_nameToTypes.end())
            return result->second;

        return Type::Undefined();
    }

    std::string_view Module::getNameByType(Type type) noexcept
    {
        auto baseType = type.toBaseType();
        auto id = baseType.id();
        if (m_typeNames.contains(id))
            return m_typeNames.at(id);
        else
            return std::string_view("undefined");
    }

    EnumDefinition& Module::createEnum(std::string_view name) noexcept
    {
        auto enumName = std::string(name);
        auto enumId = m_nextId += VariantCount;
        auto valueType = Type{ enumId, TypeKind::Enum };
        m_typeNames.try_emplace(enumId, enumName);
        m_nameToTypes.try_emplace(enumName, valueType);
        m_enumDefinitions.try_emplace(enumId, EnumDefinition{ valueType, enumName });

        return m_enumDefinitions.at(enumId);
    }

    TypeDefinition& Module::createType(std::string_view name) noexcept
    {
        auto typeName = std::string(name);
        auto typeId = m_nextId += VariantCount;
        auto valueType = Type{ typeId, TypeKind::Type };
        m_typeNames.try_emplace(typeId, typeName);
        m_nameToTypes.try_emplace(typeName, valueType);
        m_typeDefinitions.try_emplace(typeId, TypeDefinition{ valueType, typeName });

        return m_typeDefinitions.at(typeId);
    }

    FunctionDefinition& Module::createFunction(
        std::string_view name,
        const std::vector<Parameter>& parameters,
        const std::vector<Type>& returnTypes) noexcept
    {
        auto functionName = std::string(name);
        auto functionId = m_nextId += VariantCount;
        auto functionType = Type{ functionId, TypeKind::Function };
        auto isVariadic = !parameters.empty() && parameters.back().type() == Type::CVariadic();
        m_functionDefinitions.try_emplace(functionId, FunctionDefinition{ functionType, Type::Undefined(), FunctionType::FreeFunction, functionName, functionName, isVariadic, parameters, returnTypes });

        return m_functionDefinitions.at(functionId);
    }

    FunctionDefinition& Module::createMethod(
        TypeDefinition& typeDefinition,
        MethodModifier modifier,
        const std::string& methodName,
        const std::vector<Parameter>& parameters,
        const std::vector<Type>& returnTypes) noexcept
    {
        auto parentType = typeDefinition.type();
        auto fullMethodName = typeDefinition.name() + "." + methodName;
        auto methodId = m_nextId += VariantCount;
        auto valueType = Type{ methodId, TypeKind::Method };
        auto functionType = ToFunctionType(modifier);
        m_functionDefinitions.try_emplace(methodId, FunctionDefinition{ valueType, parentType, functionType, methodName, fullMethodName, false, parameters, returnTypes });
        typeDefinition.addMethod(valueType, methodName);

        return m_functionDefinitions.at(methodId);
    }

    void Module::createBuiltinType(Type type, std::string_view name, bool addVariants)
    {
        m_nameToTypes.try_emplace(std::string(name), type);
        
        if (type == Type::CVariadic())
        {
            m_typeNames.try_emplace(type.id(), std::string("C Variadic"));
        }
        else
        {
            m_typeNames.try_emplace(type.id(), std::string(name));
        }

        if (addVariants)
        {
            m_nextId += VariantCount;
        }
    }
}
