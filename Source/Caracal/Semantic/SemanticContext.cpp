#include "SemanticContext.h"

#include <Caracal/Diagnostics/DiagnosticPrinter.h>
#include <Caracal/Text/File.h>
#include <Caracal/Semantic/TypeChecker.h>
#include <Caracal/Syntax/Lexer.h>
#include <Caracal/Syntax/Parser.h>

#include <algorithm>
#include <cstdlib>
#include <iostream>

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

    std::vector<std::string> SemanticContext::CollectPreludeSources(const std::filesystem::path& preludeDirectory) noexcept
    {
        std::vector<std::filesystem::path> caraFilePaths{};
        if (std::filesystem::exists(preludeDirectory) && std::filesystem::is_directory(preludeDirectory))
        {
            for (const auto& file : std::filesystem::directory_iterator(preludeDirectory))
            {
                if (file.is_regular_file() && file.path().extension() == ".cara")
                    caraFilePaths.push_back(file.path());
            }
        }
        std::sort(caraFilePaths.begin(), caraFilePaths.end());

        std::vector<std::string> sources{};
        for (const auto& caraFilePath : caraFilePaths)
        {
            auto content = File::readText(caraFilePath);
            if (content.has_value())
                sources.push_back(std::move(content.value()));
        }

        return sources;
    }

    SemanticContext SemanticContext::WithBuiltins(const std::vector<std::string>& preludeSources, const TypeCheckerOptions& options) noexcept
    {
        SemanticContext module{};
        module.createBuiltinType(Type::CVariadic(), "...", false);
        module.createBuiltinType(Type::Function(), "function", false);
        module.createBuiltinType(Type::Discard(), "discard", false);
        module.createBuiltinType(Type::Undefined(), "undefined", false);
        module.createBuiltinType(Type::Void(), "void", false);

        module.createBuiltinType(Type::Bool(), "bool");
        module.createBuiltinType(Type::U8(), "u8");
        module.createBuiltinType(Type::I32(), "i32");
        module.createBuiltinType(Type::F32(), "f32");
        module.createBuiltinType(Type::String(), "cstring");
        module.m_nextId = 2000;

        // TODO we should move this part in the future
        // prelude is core library code, we will abort if there are errors
        DiagnosticsBag preludeDiagnostics{};
        std::vector<ParseTreeUPtr> preludeTrees{};
        for (const auto& preludeSource : preludeSources)
        {
            auto source = std::make_shared<SourceText>(preludeSource, std::filesystem::path("<prelude>"));
            const auto tokens = lex(source, preludeDiagnostics);
            preludeTrees.push_back(parse(tokens, preludeDiagnostics));
        }

        if (!preludeDiagnostics.hasErrors())
        {
            static_cast<void>(typeCheck(preludeTrees, options, module, preludeDiagnostics));
        }

        if (!preludeDiagnostics.diagnostics().empty())
        {
            std::cerr << "error: the prelude failed to compile\n";
            writeDiagnostics(std::cerr, preludeDiagnostics);
            std::abort();
        }

        module.m_preludeParseTrees = std::move(preludeTrees);

        return module;
    }

    Type SemanticContext::tryGetFunctionTypeByName(std::string_view typeName) const noexcept
    {
        for (const auto& functionDefinition : m_functionDefinitions)
        {
            // TODO add method definitions to a different list
            if (functionDefinition.functionType() != FunctionType::FreeFunction)
                continue;

            if (functionDefinition.name() == typeName)
                return functionDefinition.type();
        }

        return Type::Undefined();
    }

    EnumDefinition& SemanticContext::getEnumDefinition(Type type) noexcept
    {
        static auto invalidEnum = EnumDefinition{ nullptr, Type::Undefined(), std::string("???") };

        auto baseType = type.toBaseType();
        auto id = baseType.id();
        if (const auto index = m_enumDefinitionIndexById.find(id); index != m_enumDefinitionIndexById.end())
            return m_enumDefinitions.at(index->second);
        else
            return invalidEnum;
    }

    TypeDefinition& SemanticContext::getTypeDefinition(Type type) noexcept
    {
        static auto invalidType = TypeDefinition{ nullptr, Type::Undefined(), std::string("???") };

        auto baseType = type.toBaseType();
        auto id = baseType.id();
        if (const auto index = m_typeDefinitionIndexById.find(id); index != m_typeDefinitionIndexById.end())
            return m_typeDefinitions.at(index->second);
        else
            return invalidType;
    }

    const OperatorSignature* SemanticContext::tryGetOperatorSignature(Type type, BinaryOperatorKind operation) const noexcept
    {
        if (!type.isBaseType())
        {
            return nullptr;
        }

        const auto index = m_typeDefinitionIndexById.find(type.id());
        if (index == m_typeDefinitionIndexById.end())
        {
            return nullptr;
        }

        return m_typeDefinitions.at(index->second).tryGetOperatorSignature(operation);
    }

    const OperatorSignature* SemanticContext::tryGetOperatorSignature(Type type, UnaryOperatorKind operation) const noexcept
    {
        if (!type.isBaseType())
        {
            return nullptr;
        }

        const auto index = m_typeDefinitionIndexById.find(type.id());
        if (index == m_typeDefinitionIndexById.end())
        {
            return nullptr;
        }

        return m_typeDefinitions.at(index->second).tryGetOperatorSignature(operation);
    }

    FunctionDefinition& SemanticContext::getFunctionDefinition(Type type) noexcept
    {
        static auto invalidFunction = FunctionDefinition{ nullptr, Type::Undefined(), Type::Undefined(), FunctionType::None, std::string("???"), std::string("???"), false };

        auto id = type.id();
        if (const auto index = m_functionDefinitionIndexById.find(id); index != m_functionDefinitionIndexById.end())
            return m_functionDefinitions.at(index->second);
        else
            return invalidFunction;
    }

    Type SemanticContext::tryGetTypeByName(std::string_view name) const noexcept
    {
        if (const auto result = m_nameToTypes.find(std::string(name)); result != m_nameToTypes.end())
            return result->second;

        return Type::Undefined();
    }

    std::string_view SemanticContext::getNameByType(Type type) const noexcept
    {
        auto baseType = type.toBaseType();
        auto id = baseType.id();
        if (m_typeNames.contains(id))
            return m_typeNames.at(id);
        else
            return std::string_view("undefined");
    }

    [[nodiscard]] static std::string BuildArrayTypeName(
        const SemanticContext& module,
        TypeKind arrayKind,
        Type elementType,
        i32 length)
    {
        auto elementName = std::string(module.getNameByType(elementType));
        if (elementType.isReference())
        {
            elementName = "ref " + elementName;
        }

        auto name = "[" + elementName;
        if (arrayKind == TypeKind::FixedArray)
        {
            name += "; " + std::to_string(length);
        }
        else if (arrayKind == TypeKind::DynamicArray)
        {
            name += "; _";
        }

        name += "]";
        return name;
    }

    Type SemanticContext::getOrCreateArrayType(TypeKind arrayKind, Type elementType, i32 length) noexcept
    {
        auto arrayName = BuildArrayTypeName(*this, arrayKind, elementType, length);
        if (const auto existing = m_nameToTypes.find(arrayName); existing != m_nameToTypes.end())
        {
            return existing->second;
        }

        auto arrayId = m_nextId += VariantCount;
        auto arrayType = Type{ arrayId, arrayKind };
        m_typeNames.try_emplace(arrayId, arrayName);
        m_nameToTypes.try_emplace(arrayName, arrayType);
        m_arrayTypeInfoById.try_emplace(arrayId, ArrayTypeInfo{ elementType, length });

        return arrayType;
    }

    Type SemanticContext::getArrayElementType(Type type) const noexcept
    {
        const auto id = type.toBaseType().id();
        if (const auto info = m_arrayTypeInfoById.find(id); info != m_arrayTypeInfoById.end())
        {
            return info->second.elementType;
        }

        return Type::Undefined();
    }

    i32 SemanticContext::getArrayLength(Type type) const noexcept
    {
        const auto id = type.toBaseType().id();
        if (const auto info = m_arrayTypeInfoById.find(id); info != m_arrayTypeInfoById.end())
        {
            return info->second.length;
        }

        return 0;
    }

    EnumDefinition& SemanticContext::createEnum(
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

    TypeDefinition& SemanticContext::createType(
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

    TypeDefinition& SemanticContext::bindBuiltinTypeDefinition(Type type, const TypeDefinitionStatement* statement) noexcept
    {
        const auto typeId = type.id();
        m_typeDefinitions.emplace_back(statement, type, m_typeNames.at(typeId));
        m_typeDefinitionIndexById.try_emplace(typeId, m_typeDefinitions.size() - 1);

        return m_typeDefinitions.back();
    }

    FunctionDefinition& SemanticContext::createFunction(
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

    FunctionDefinition& SemanticContext::createMethod(
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

    FunctionDefinition& SemanticContext::createConstructor(
        TypeDefinition& typeDefinition,
        const std::vector<Parameter>& parameters) noexcept
    {
        auto parentType = typeDefinition.type();
        auto constructorName = std::string("new");
        auto fullConstructorName = typeDefinition.name() + "." + constructorName;
        auto constructorId = m_nextId += VariantCount;
        auto methodType = Type{ constructorId, TypeKind::Constructor };
        m_functionDefinitions.emplace_back(nullptr, methodType, parentType, FunctionType::SynthesizedConstructor, constructorName, fullConstructorName, false, parameters, std::vector<Type>{ Type::Void() });
        m_functionDefinitionIndexById.try_emplace(constructorId, m_functionDefinitions.size() - 1);
        typeDefinition.addMethod(methodType, constructorName);

        return m_functionDefinitions.back();
    }

    ConstantDefinition& SemanticContext::createConstant(
        std::string_view name,
        Expression* expression) noexcept
    {
        auto constantName = std::string(name);
        m_constantDefinitions.emplace_back(constantName, expression);
        m_constantDefinitionIndexByName.try_emplace(constantName, m_constantDefinitions.size() - 1);

        return m_constantDefinitions.back();
    }

    ConstantDefinition& SemanticContext::createInitConstant(
        std::string_view name,
        Type type) noexcept
    {
        auto constantName = std::string(name);
        m_constantDefinitions.emplace_back(constantName, type);
        m_constantDefinitionIndexByName.try_emplace(constantName, m_constantDefinitions.size() - 1);

        return m_constantDefinitions.back();
    }

    void SemanticContext::addGlobalDiscardExpression(const Expression* expression) noexcept
    {
        m_globalDiscardExpressions.push_back(expression);
    }

    void SemanticContext::createBuiltinType(Type type, std::string_view name, bool addVariants)
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
