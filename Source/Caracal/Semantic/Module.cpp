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
        module.m_functionDefinitions.emplace_back(nullptr, Type{ 1000, TypeKind::Function }, Type::Undefined(), FunctionType::FreeFunction, "print", "print", false, std::vector<Parameter>{Parameter{"msg", Type::String()}});
        module.m_functionDefinitionIndexById.try_emplace(1000, module.m_functionDefinitions.size() - 1);
        module.m_nextId = 2000;

        return module;
    }

    Type Module::tryGetFunctionTypeByName(std::string_view typeName) const noexcept
    {
        for (const auto& functionDefinition : m_functionDefinitions)
        {
            if (functionDefinition.name() == typeName)
                return functionDefinition.type();
        }

        return Type::Undefined();
    }

    EnumDefinition& Module::getEnumDefinition(Type type) noexcept
    {
        static auto invalidEnum = EnumDefinition{ nullptr, Type::Undefined(), std::string("???") };

        auto baseType = type.toBaseType();
        auto id = baseType.id();
        if (const auto index = m_enumDefinitionIndexById.find(id); index != m_enumDefinitionIndexById.end())
            return m_enumDefinitions.at(index->second);
        else
            return invalidEnum;
    }

    TypeDefinition& Module::getTypeDefinition(Type type) noexcept
    {
        static auto invalidType = TypeDefinition{ nullptr, Type::Undefined(), std::string("???") };

        auto baseType = type.toBaseType();
        auto id = baseType.id();
        if (const auto index = m_typeDefinitionIndexById.find(id); index != m_typeDefinitionIndexById.end())
            return m_typeDefinitions.at(index->second);
        else
            return invalidType;
    }

    FunctionDefinition& Module::getFunctionDefinition(Type type) noexcept
    {
        static auto invalidFunction = FunctionDefinition{ nullptr, Type::Undefined(), Type::Undefined(), FunctionType::None, std::string("???"), std::string("???"), false };

        auto id = type.id();
        if (const auto index = m_functionDefinitionIndexById.find(id); index != m_functionDefinitionIndexById.end())
            return m_functionDefinitions.at(index->second);
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

    EnumDefinition& Module::createEnum(
        std::string_view name,
        const EnumDefinitionStatement* statement) noexcept
    {
        auto enumName = std::string(name);
        auto enumId = m_nextId += VariantCount;
        auto valueType = Type{ enumId, TypeKind::Enum };
        m_typeNames.try_emplace(enumId, enumName);
        m_nameToTypes.try_emplace(enumName, valueType);
        m_enumDefinitions.emplace_back(statement, valueType, enumName);
        m_enumDefinitionIndexById.try_emplace(enumId, m_enumDefinitions.size() - 1);

        return m_enumDefinitions.back();
    }

    TypeDefinition& Module::createType(
        std::string_view name,
        const TypeDefinitionStatement* statement) noexcept
    {
        auto typeName = std::string(name);
        auto typeId = m_nextId += VariantCount;
        auto valueType = Type{ typeId, TypeKind::Type };
        m_typeNames.try_emplace(typeId, typeName);
        m_nameToTypes.try_emplace(typeName, valueType);
        m_typeDefinitions.emplace_back(statement, valueType, typeName);
        m_typeDefinitionIndexById.try_emplace(typeId, m_typeDefinitions.size() - 1);

        return m_typeDefinitions.back();
    }

    FunctionDefinition& Module::createFunction(
        std::string_view name,
        const std::vector<Parameter>& parameters,
        const std::vector<Type>& returnTypes,
        const FunctionDefinitionStatement* functionStatement) noexcept
    {
        auto functionName = std::string(name);
        auto functionId = m_nextId += VariantCount;
        auto functionType = Type{ functionId, TypeKind::Function };
        auto isVariadic = !parameters.empty() && parameters.back().type() == Type::CVariadic();
        const Statement* statement = static_cast<const Statement*>(functionStatement);
        m_functionDefinitions.emplace_back(statement, functionType, Type::Undefined(), FunctionType::FreeFunction, functionName, functionName, isVariadic, parameters, returnTypes);
        m_functionDefinitionIndexById.try_emplace(functionId, m_functionDefinitions.size() - 1);

        return m_functionDefinitions.back();
    }

    FunctionDefinition& Module::createMethod(
        TypeDefinition& typeDefinition,
        MethodModifier modifier,
        const std::string& methodName,
        const std::vector<Parameter>& parameters,
        const std::vector<Type>& returnTypes,
        const MethodDefinitionStatement* methodStatement) noexcept
    {
        auto parentType = typeDefinition.type();
        auto fullMethodName = typeDefinition.name() + "." + methodName;
        auto methodId = m_nextId += VariantCount;
        auto methodType = Type{ methodId, TypeKind::Method };
        auto functionType = ToFunctionType(modifier);
        const Statement* statement = static_cast<const Statement*>(methodStatement);
        m_functionDefinitions.emplace_back(statement, methodType, parentType, functionType, methodName, fullMethodName, false, parameters, returnTypes);
        m_functionDefinitionIndexById.try_emplace(methodId, m_functionDefinitions.size() - 1);
        typeDefinition.addMethod(methodType, methodName);

        return m_functionDefinitions.back();
    }

    FunctionDefinition& Module::createConstructor(
        TypeDefinition& typeDefinition,
        const std::vector<Parameter>& parameters) noexcept
    {
        auto parentType = typeDefinition.type();
        auto constructorName = std::string("new");
        auto fullConstructorName = typeDefinition.name() + "." + constructorName;
        auto constructorId = m_nextId += VariantCount;
        auto methodType = Type{ constructorId, TypeKind::Constructor };
        m_functionDefinitions.emplace_back(nullptr, methodType, parentType, FunctionType::Constructor, constructorName, fullConstructorName, false, parameters, std::vector<Type>{ Type::Void() });
        m_functionDefinitionIndexById.try_emplace(constructorId, m_functionDefinitions.size() - 1);
        typeDefinition.addMethod(methodType, constructorName);

        return m_functionDefinitions.back();
    }

    ConstantDefinition& Module::createConstant(
        std::string_view name,
        Expression* expression) noexcept
    {
        auto constantName = std::string(name);
        m_constantDefinitions.emplace_back(constantName, expression);
        m_constantDefinitionIndexByName.try_emplace(constantName, m_constantDefinitions.size() - 1);

        return m_constantDefinitions.back();
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
