#include "TypeChecker.h"

#include <Caracal/Constants.h>
#include <Caracal/Semantic/ArgumentBinder.h>
#include <Caracal/Syntax/BoolLiteral.h>
#include <Caracal/Syntax/BreakStatement.h>
#include <Caracal/Syntax/GroupingExpression.h>
#include <Caracal/Syntax/SkipStatement.h>
#include <Caracal/Syntax/StringLiteral.h>

#include <algorithm>
#include <charconv>
#include <cmath>
#include <limits>

namespace Caracal
{
    struct AnnotationDefinition
    {
        AnnotationKind kind;
        std::string_view name;
        TokenKind targetKind;
        i32 requiredArgumentCount;
        Type parameterType;
        std::string_view namedStringArgument;
    };

    static const AnnotationDefinition* GetAnnotationDefinition(AnnotationKind kind)
    {
        static const AnnotationDefinition Definitions[] = {
            { AnnotationKind::Extern, "extern", TokenKind::DefKeyword, 0, Type::Undefined(), "symbol" },
            { AnnotationKind::Flag, "flag", TokenKind::EnumKeyword, 0, Type::Undefined(), "" },
            { AnnotationKind::Step, "step", TokenKind::EnumKeyword, 1, Type::I32(), "" },
            { AnnotationKind::Builtin, "builtin", TokenKind::TypeKeyword, 0, Type::Undefined(), "" },
        };

        for (const auto& definition : Definitions)
        {
            if (definition.kind == kind)
            {
                return &definition;
            }
        }

        return nullptr;
    }

    struct BuiltinOperatorDefinition
    {
        std::string_view methodName;
        bool isUnary;
        BinaryOperatorKind binaryOperator;
        UnaryOperatorKind unaryOperator;
        BinaryFoldFunction binaryFold;
        UnaryFoldFunction unaryFold;
    };

    static const BuiltinOperatorDefinition* TryGetBuiltinOperatorDefinition(std::string_view methodName)
    {
        static constexpr BuiltinOperatorDefinition Definitions[] = {
            { "add", false, BinaryOperatorKind::Addition, UnaryOperatorKind::Invalid, &FoldAddition, nullptr },
            { "subtract", false, BinaryOperatorKind::Subtraction, UnaryOperatorKind::Invalid, &FoldSubtraction, nullptr },
            { "multiply", false, BinaryOperatorKind::Multiplication, UnaryOperatorKind::Invalid, &FoldMultiplication, nullptr },
            { "divide", false, BinaryOperatorKind::Division, UnaryOperatorKind::Invalid, &FoldDivision, nullptr },
            { "equals", false, BinaryOperatorKind::Equal, UnaryOperatorKind::Invalid, nullptr, nullptr },
            { "notEquals", false, BinaryOperatorKind::NotEqual, UnaryOperatorKind::Invalid, nullptr, nullptr },
            { "lessThan", false, BinaryOperatorKind::LessThan, UnaryOperatorKind::Invalid, nullptr, nullptr },
            { "lessOrEqual", false, BinaryOperatorKind::LessOrEqual, UnaryOperatorKind::Invalid, nullptr, nullptr },
            { "greaterThan", false, BinaryOperatorKind::GreaterThan, UnaryOperatorKind::Invalid, nullptr, nullptr },
            { "greaterOrEqual", false, BinaryOperatorKind::GreaterOrEqual, UnaryOperatorKind::Invalid, nullptr, nullptr },
            { "logicalAnd", false, BinaryOperatorKind::LogicalAnd, UnaryOperatorKind::Invalid, nullptr, nullptr },
            { "logicalOr", false, BinaryOperatorKind::LogicalOr, UnaryOperatorKind::Invalid, nullptr, nullptr },
            { "negate", true, BinaryOperatorKind::Invalid, UnaryOperatorKind::ValueNegation, nullptr, &FoldValueNegation },
            { "logicalNegate", true, BinaryOperatorKind::Invalid, UnaryOperatorKind::LogicalNegation, nullptr, &FoldLogicalNegation },
        };

        for (const auto& definition : Definitions)
        {
            if (definition.methodName == methodName)
            {
                return &definition;
            }
        }

        return nullptr;
    }

    static std::optional<SourceLocation> GetTypeFieldLocation(const TypeDefinition& typeDefinition, const FieldDefinition& fieldDefinition, const TokenBuffer& tokens)
    {
        const auto fieldIndex = static_cast<size_t>(fieldDefinition.index());
        const auto& statements = typeDefinition.statement()->bodyNode()->statements();
        if (fieldIndex >= statements.size())
        {
            return std::nullopt;
        }

        auto* fieldStatement = static_cast<TypeFieldDeclaration*>(statements[fieldIndex].get());
        return fieldStatement->nameExpression()->sourceLocation(tokens);
    }

    static std::string FormatTypeName(SemanticContext& module, Type type)
    {
        if (type == Type::Undefined())
        {
            return "undefined";
        }

        if (type == Type::Void())
        {
            return "void";
        }

        auto name = std::string(module.getNameByType(type));
        if (type.isReference())
        {
            return "ref " + name;
        }

        return name;
    }

    static bool ShouldIgnoreUnusedVariableWarning(std::string_view name)
    {
        return name == "_" || name == ImplicitThisName;
    }

    static bool IsInitConstantAssignmentSite(std::string_view functionName)
    {
        return functionName == EntryPointFunctionName || functionName == UserMainFunctionName;
    }

    static std::string FormatBinaryOperator(BinaryOperatorKind binaryOperator)
    {
        switch (binaryOperator)
        {
            case BinaryOperatorKind::Addition:
                return "+";
            case BinaryOperatorKind::Subtraction:
                return "-";
            case BinaryOperatorKind::Multiplication:
                return "*";
            case BinaryOperatorKind::Division:
                return "/";
            case BinaryOperatorKind::Equal:
                return "==";
            case BinaryOperatorKind::NotEqual:
                return "!=";
            case BinaryOperatorKind::LessThan:
                return "<";
            case BinaryOperatorKind::LessOrEqual:
                return "<=";
            case BinaryOperatorKind::GreaterThan:
                return ">";
            case BinaryOperatorKind::GreaterOrEqual:
                return ">=";
            case BinaryOperatorKind::LogicalAnd:
                return "and";
            case BinaryOperatorKind::LogicalOr:
                return "or";
            default:
                return stringify(binaryOperator);
        }
    }

    static std::optional<i32> TryParseI32Literal(std::string_view lexeme)
    {
        i32 value = 0;
        const auto* begin = lexeme.data();
        const auto* end = begin + lexeme.size();
        const auto result = std::from_chars(begin, end, value);
        if (result.ec != std::errc{} || result.ptr != end)
        {
            return std::nullopt;
        }

        return value;
    }

    static std::optional<i32> TryParseNegatedI32Literal(std::string_view lexeme)
    {
        std::int64_t magnitude = 0;
        const auto* begin = lexeme.data();
        const auto* end = begin + lexeme.size();
        const auto result = std::from_chars(begin, end, magnitude);
        if (result.ec != std::errc{} || result.ptr != end)
        {
            return std::nullopt;
        }

        const auto negated = -magnitude;
        if (negated < static_cast<std::int64_t>(std::numeric_limits<i32>::min())
            || negated > static_cast<std::int64_t>(std::numeric_limits<i32>::max()))
        {
            return std::nullopt;
        }

        return static_cast<i32>(negated);
    }

    static std::optional<u8> TryParseU8Literal(std::string_view lexeme)
    {
        unsigned int value = 0;
        const auto* begin = lexeme.data();
        const auto* end = begin + lexeme.size();
        const auto result = std::from_chars(begin, end, value);
        if (result.ec != std::errc{} || result.ptr != end || value > std::numeric_limits<u8>::max())
        {
            return std::nullopt;
        }

        return static_cast<u8>(value);
    }

    static std::optional<float> TryParseF32Literal(std::string_view lexeme)
    {
        float value = 0.0f;
        const auto* begin = lexeme.data();
        const auto* end = begin + lexeme.size();
        const auto result = std::from_chars(begin, end, value);
        if (result.ec != std::errc{} || result.ptr != end || !std::isfinite(value))
        {
            return std::nullopt;
        }

        return value;
    }

    static bool DoesLiteralFitType(std::string_view lexeme, Type type)
    {
        if (type == Type::U8())
        {
            return TryParseU8Literal(lexeme).has_value();
        }
        else if (type == Type::I32())
        {
            return TryParseI32Literal(lexeme).has_value();
        }
        else if (type == Type::F32())
        {
            return TryParseF32Literal(lexeme).has_value();
        }

        return true;
    }

    static std::optional<NumberLiteral::ParsedValue> TryParseNumberLiteralValue(std::string_view lexeme, Type type)
    {
        if (type == Type::U8())
        {
            const auto value = TryParseU8Literal(lexeme);
            if (!value.has_value())
                return std::nullopt;

            return NumberLiteral::ParsedValue{ value.value() };
        }

        if (type == Type::I32())
        {
            const auto value = TryParseI32Literal(lexeme);
            if (!value.has_value())
                return std::nullopt;

            return NumberLiteral::ParsedValue{ value.value() };
        }

        if (type == Type::F32())
        {
            const auto value = TryParseF32Literal(lexeme);
            if (!value.has_value())
                return std::nullopt;

            return NumberLiteral::ParsedValue{ value.value() };
        }

        return std::nullopt;
    }

    static std::optional<i32> TryConvertEnumFieldLiteralValue(const NumberLiteral& literal)
    {
        if (!literal.hasParsedValue())
        {
            return std::nullopt;
        }

        const auto literalType = literal.type();
        if (literalType == Type::U8())
        {
            return static_cast<i32>(std::get<u8>(literal.parsedValue().value()));
        }

        if (literalType == Type::I32())
        {
            return std::get<i32>(literal.parsedValue().value());
        }

        return std::nullopt;
    }

    bool typeCheck(
        const std::vector<ParseTreeUPtr>& parseTrees,
        const TypeCheckerOptions& options,
        SemanticContext& module,
        DiagnosticsBag& diagnostics) noexcept
    {
        TypeChecker typeChecker{ parseTrees, options, module, diagnostics };
        return typeChecker.typeCheck();
    }

    TypeChecker::TypeChecker(
        const std::vector<ParseTreeUPtr>& parseTrees,
        const TypeCheckerOptions& options,
        SemanticContext& module,
        DiagnosticsBag& diagnostics)
        : m_parseTrees(parseTrees)
        , m_options{ options }
        , m_module{ module }
        , m_diagnostics{ diagnostics }
        , m_currentReturnType{ Type::Void() }
        , m_currentType{ Type::Undefined() }
        , m_scopes{}
    {
        m_scopes.emplace_back(std::make_unique<Scope>(nullptr, ScopeKind::Global));
    }

    bool TypeChecker::typeCheck()
    {
        if (m_diagnostics.hasErrors())
        {
            return false;
        }

        collectDeclarations();
        collectMethodDeclarations();

        typeCheckFunctionSignatures();
        typeCheckTypeSignatures();
        typeCheckEnumDefinitions();
        typeCheckGlobalConstants();
        typeCheckTypeFieldDefinitions();
        typeCheckFunctionDefinitions();
        typeCheckTypeMethodDefinitions();
        
        checkUninitializedInitConstants();

        return !m_diagnostics.hasErrors();
    }

    void TypeChecker::checkUninitializedInitConstants()
    {
        for (const auto& [name, declaration] : m_initConstants)
        {
            if (m_initConstantAssignments.contains(name))
                continue;

            m_diagnostics.addUninitializedInitConstantError(declaration.source, declaration.location, name);
        }
    }

    void TypeChecker::collectDeclarations()
    {
        for (const auto& parseTree : m_parseTrees)
        {
            for (const auto& statement : parseTree->statements())
            {
                switch (statement->kind())
                {
                    case NodeKind::ConstantDeclaration:
                    {
                        auto* constantDeclaration = static_cast<ConstantDeclaration*>(statement.get());
                        m_statementTokens.emplace(constantDeclaration, &parseTree->tokens());
                        m_globalConstantDeclarations.push_back(constantDeclaration);

                        break;
                    }
                    case NodeKind::VariableDeclaration:
                    {
                        auto* variableDeclaration = static_cast<VariableDeclaration*>(statement.get());
                        if (!variableDeclaration->annotations().empty())
                        {
                            for (const auto& annotation : variableDeclaration->annotations())
                            {
                                m_diagnostics.addUnexpectedAnnotationTargetError(
                                    parseTree->tokens().source(),
                                    annotation->sourceLocation(parseTree->tokens()));
                            }
                        }

                        break;
                    }
                    case NodeKind::EnumDefinitionStatement:
                    {
                        auto* enumStatement = static_cast<EnumDefinitionStatement*>(statement.get());
                        m_statementTokens.emplace(enumStatement, &parseTree->tokens());
                        const auto& enumName = enumStatement->name();
                        auto existingType = m_module.tryGetTypeByName(enumName);
                        if (existingType != Type::Undefined())
                        {
                            auto otherSource = SourceTextSharedPtr{};
                            auto otherLocation = std::optional<SourceLocation>{};
                            if (existingType.kind() == TypeKind::Enum)
                            {
                                auto* otherStatement = m_module.getEnumDefinition(existingType).statement();
                                if (otherStatement != nullptr)
                                {
                                    const auto& otherTokens = tokensFor(otherStatement);
                                    otherSource = otherTokens.source();
                                    otherLocation = otherTokens.getSourceLocation(otherStatement->nameToken());
                                }
                            }
                            else if (existingType.kind() == TypeKind::Type)
                            {
                                auto* otherStatement = m_module.getTypeDefinition(existingType).statement();
                                if (otherStatement != nullptr)
                                {
                                    const auto& otherTokens = tokensFor(otherStatement);
                                    otherSource = otherTokens.source();
                                    otherLocation = otherTokens.getSourceLocation(otherStatement->nameToken());
                                }
                            }

                            m_diagnostics.addDuplicateTypeDeclarationError(
                                parseTree->tokens().source(),
                                parseTree->tokens().getSourceLocation(enumStatement->nameToken()),
                                enumName,
                                otherSource,
                                otherLocation);
                            break;
                        }

                        auto& enumDefinition = m_module.createEnum(enumName, enumStatement);
                        enumStatement->setType(enumDefinition.type());
                        m_enumDeclarations.push_back(enumStatement);

                        break;
                    }
                    case NodeKind::TypeDefinitionStatement:
                    {
                        auto* typeStatement = static_cast<TypeDefinitionStatement*>(statement.get());
                        m_statementTokens.emplace(typeStatement, &parseTree->tokens());
                        if (typeStatement->isBuiltin())
                        {
                            const auto builtinType = m_module.tryGetTypeByName(typeStatement->name());
                            if (builtinType.kind() != TypeKind::Builtin || builtinType.id() < 0)
                            {
                                m_diagnostics.addNotABuiltinTypeError(
                                    parseTree->tokens().source(),
                                    parseTree->tokens().getSourceLocation(typeStatement->nameToken()),
                                    std::string(typeStatement->name()));
                                break;
                            }

                            const auto& existingDefinition = m_module.getTypeDefinition(builtinType);
                            if (existingDefinition.type() != Type::Undefined())
                            {
                                auto otherLocation = std::optional<SourceLocation>{};
                                const auto* otherStatement = existingDefinition.statement();
                                if (otherStatement != nullptr)
                                {
                                    const auto* otherTokens = tryTokensFor(otherStatement);
                                    if (otherTokens != nullptr)
                                    {
                                        otherLocation = otherTokens->getSourceLocation(otherStatement->nameToken());
                                    }
                                }

                                m_diagnostics.addDuplicateBuiltinTypeBindingError(
                                    parseTree->tokens().source(),
                                    parseTree->tokens().getSourceLocation(typeStatement->nameToken()),
                                    std::string(typeStatement->name()),
                                    otherLocation);
                                break;
                            }

                            m_module.bindBuiltinTypeDefinition(builtinType, typeStatement);
                            typeStatement->setType(builtinType);
                            m_typeDeclarations.push_back(typeStatement);
                            break;
                        }

                        auto existingType = m_module.tryGetTypeByName(typeStatement->name());
                        if (existingType != Type::Undefined())
                        {
                            auto otherSource = SourceTextSharedPtr{};
                            auto otherLocation = std::optional<SourceLocation>{};
                            if (existingType.kind() == TypeKind::Enum)
                            {
                                auto* otherStatement = m_module.getEnumDefinition(existingType).statement();
                                if (otherStatement != nullptr)
                                {
                                    const auto& otherTokens = tokensFor(otherStatement);
                                    otherSource = otherTokens.source();
                                    otherLocation = otherTokens.getSourceLocation(otherStatement->nameToken());
                                }
                            }
                            else if (existingType.kind() == TypeKind::Type)
                            {
                                auto* otherStatement = m_module.getTypeDefinition(existingType).statement();
                                if (otherStatement != nullptr)
                                {
                                    const auto& otherTokens = tokensFor(otherStatement);
                                    otherSource = otherTokens.source();
                                    otherLocation = otherTokens.getSourceLocation(otherStatement->nameToken());
                                }
                            }

                            m_diagnostics.addDuplicateTypeDeclarationError(
                                parseTree->tokens().source(),
                                parseTree->tokens().getSourceLocation(typeStatement->nameToken()),
                                std::string(typeStatement->name()),
                                otherSource,
                                otherLocation);
                            break;
                        }

                        auto& typeDefinition = m_module.createType(typeStatement->name(), typeStatement);
                        typeStatement->setType(typeDefinition.type());
                        m_typeDeclarations.push_back(typeStatement);

                        break;
                    }
                    case NodeKind::FunctionDefinitionStatement:
                    {
                        auto* functionStatement = static_cast<FunctionDefinitionStatement*>(statement.get());
                        m_statementTokens.emplace(functionStatement, &parseTree->tokens());
                        const auto& functionName = functionStatement->name();
                        auto existingFunctionType = m_module.tryGetFunctionTypeByName(functionName);
                        if (existingFunctionType != Type::Undefined())
                        {
                            auto otherSource = SourceTextSharedPtr{};
                            auto otherLocation = std::optional<SourceLocation>{};
                            auto* otherStatement = static_cast<const FunctionDefinitionStatement*>(m_module.getFunctionDefinition(existingFunctionType).statement());
                            if (otherStatement != nullptr)
                            {
                                const auto& otherTokens = tokensFor(otherStatement);
                                otherSource = otherTokens.source();
                                otherLocation = otherTokens.getSourceLocation(otherStatement->nameToken());
                            }

                            m_diagnostics.addDuplicateFunctionDeclarationError(
                                parseTree->tokens().source(),
                                parseTree->tokens().getSourceLocation(functionStatement->nameToken()),
                                functionName,
                                otherSource,
                                otherLocation);
                            break;
                        }

                        std::vector<Parameter> parameters{};
                        const auto& parametersNodes = functionStatement->parametersNode()->parameters();
                        for (const auto& parameterNode : parametersNodes)
                        {
                            parameters.emplace_back(parameterNode->name(), Type::Undefined());
                        }

                        auto& functionDefinition = m_module.createFunction(functionName, parameters, std::vector<Type>(), functionStatement);
                        functionStatement->setType(functionDefinition.type());
                        m_functionDeclarations.push_back(functionStatement);

                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
            }
        }
    }

    void TypeChecker::collectMethodDeclarations()
    {
        for (const auto* typeDefinitionStatement : m_typeDeclarations)
        {
            const auto typeType = m_module.tryGetTypeByName(typeDefinitionStatement->name());
            if (typeType == Type::Undefined())
            {
                TODO("This shouldn't happen");
            }

            auto& typeDefinition = m_module.getTypeDefinition(typeType);
            const auto& bodyStatements = typeDefinitionStatement->bodyNode()->statements();
            for (const auto& bodyStatement : bodyStatements)
            {
                if (bodyStatement->kind() != NodeKind::MethodDefinitionStatement)
                {
                    continue;
                }

                auto* methodStatement = static_cast<MethodDefinitionStatement*>(bodyStatement.get());
                const auto modifier = methodStatement->modifier();
                const auto& methodName = methodStatement->methodNameNode()->methodName();
                if (methodStatement->specialFunctionType() == SpecialFunctionType::Constructor || methodName == "new")
                {
                    const auto& tokens = tokensFor(typeDefinitionStatement);
                    auto constructorLocation = tokens.getSourceLocation(typeDefinitionStatement->nameToken());
                    if (typeDefinitionStatement->constructorParameters().has_value())
                    {
                        constructorLocation = typeDefinitionStatement->constructorParameters().value()->sourceLocation(tokens);
                    }

                    m_diagnostics.addTypeDotNewDeclarationError(
                        tokens.source(),
                        tokens.getSourceLocation(methodStatement->methodNameNode()->methodNameToken()),
                        constructorLocation,
                        std::string(typeDefinitionStatement->name()));
                    continue;
                }

                std::vector<Parameter> declarationParameters{};
                if (modifier != MethodModifier::Static)
                {
                    declarationParameters.emplace_back(ImplicitThisName, typeDefinition.type().toReference());
                }

                const auto& parameterNodes = methodStatement->parametersNode()->parameters();
                for (const auto& parameterNode : parameterNodes)
                {
                    declarationParameters.emplace_back(parameterNode->name(), Type::Undefined());
                }

                auto methodType = typeDefinition.tryGetMethodTypeByName(methodName);
                if (methodType != Type::Undefined())
                {
                    continue;
                }

                auto& methodDefinition = m_module.createMethod(typeDefinition, modifier, methodName, declarationParameters, std::vector<Type>(), methodStatement);
                methodStatement->setType(methodDefinition.type());
            }

            if (typeDefinitionStatement->isBuiltin())
            {
                continue;
            }

            std::vector<Parameter> constructorParameters{};
            constructorParameters.emplace_back(ImplicitThisName, typeType.toReference());

            if (typeDefinitionStatement->constructorParameters().has_value())
            {
                const auto& parameterNodes = typeDefinitionStatement->constructorParameters().value()->parameters();
                for (const auto& parameterNode : parameterNodes)
                {
                    constructorParameters.emplace_back(parameterNode->name(), Type::Undefined());
                }
            }

            static_cast<void>(m_module.createConstructor(typeDefinition, constructorParameters));
        }
    }

    void TypeChecker::typeCheckFunctionSignatures()
    {
        for (const auto* functionDefinitionStatement : m_functionDeclarations)
        {
            auto* statement = const_cast<FunctionDefinitionStatement*>(functionDefinitionStatement);
            typeCheckFunctionSignature(statement, tokensFor(statement));
        }
    }

    void TypeChecker::typeCheckTypeSignatures()
    {
        for (const auto* typeDefinitionStatement : m_typeDeclarations)
        {
            auto* statement = const_cast<TypeDefinitionStatement*>(typeDefinitionStatement);
            typeCheckTypeSignature(statement, tokensFor(statement));
        }
    }

    void TypeChecker::typeCheckGlobalConstants()
    {
        for (const auto* constantDeclaration : m_globalConstantDeclarations)
        {
            auto* statement = const_cast<ConstantDeclaration*>(constantDeclaration);
            typeCheckConstantDeclaration(statement, tokensFor(statement));
        }
    }

    void TypeChecker::typeCheckFunctionDefinitions()
    {
        for (const auto* functionDefinitionStatement : m_functionDeclarations)
        {
            auto* statement = const_cast<FunctionDefinitionStatement*>(functionDefinitionStatement);
            typeCheckFunctionDefinitionStatement(statement, tokensFor(statement));
        }
    }

    void TypeChecker::typeCheckEnumDefinitions()
    {
        for (const auto* enumDefinitionStatement : m_enumDeclarations)
        {
            auto* statement = const_cast<EnumDefinitionStatement*>(enumDefinitionStatement);
            typeCheckEnumDefinitionStatement(statement, tokensFor(statement));
        }
    }

    void TypeChecker::typeCheckTypeFieldDefinitions()
    {
        for (const auto* typeDefinitionStatement : m_typeDeclarations)
        {
            auto* statement = const_cast<TypeDefinitionStatement*>(typeDefinitionStatement);
            typeCheckTypeFieldDefinition(statement, tokensFor(statement));
        }
    }

    void TypeChecker::typeCheckTypeMethodDefinitions()
    {
        for (const auto* typeDefinitionStatement : m_typeDeclarations)
        {
            auto* statement = const_cast<TypeDefinitionStatement*>(typeDefinitionStatement);
            typeCheckTypeMethodDefinition(statement, tokensFor(statement));
        }
    }

    void TypeChecker::typeCheckFunctionSignature(FunctionDefinitionStatement* statement, const TokenBuffer& tokens)
    {
        pushScope(ScopeKind::Function);
        auto parameters = typeCheckParametersNode(statement->parametersNode().get(), tokens);
        auto returns = typeCheckReturnTypesNode(statement->returnTypesNode().get(), tokens);
        popScope(false);

        const auto& functionName = statement->name();
        auto functionType = m_module.tryGetFunctionTypeByName(functionName);
        if (functionType == Type::Undefined())
        {
            TODO("This shouldn't happen");
        }

        auto& functionDefinition = m_module.getFunctionDefinition(functionType);
        const auto& parameterNodes = statement->parametersNode()->parameters();
        std::optional<std::string> symbolName;
        const auto isExtern = validateCallableAnnotations(statement->annotations(), tokens, symbolName);
        const auto isVariadic = !parameterNodes.empty() && parameterNodes.back()->isVariadic();
        if (isVariadic && !isExtern)
        {
            auto* variadicParameter = parameterNodes.back().get();
            m_diagnostics.addNonExternVariadicFunctionError(
                tokens.source(),
                variadicParameter->sourceLocation(tokens),
                functionName);
        }
        functionDefinition.setParameters(parameters);
        functionDefinition.setReturnTypes(returns);
        functionDefinition.setIsVariadic(isVariadic);
        functionDefinition.setSymbolName(std::move(symbolName));
    }

    void TypeChecker::typeCheckTypeSignature(TypeDefinitionStatement* statement, const TokenBuffer& tokens)
    {
        auto typeType = statement->type();
        if (typeType == Type::Undefined())
        {
            TODO("This shouldn't happen");
        }

        validateTypeAnnotation(statement, tokens);

        auto& typeDefinition = m_module.getTypeDefinition(typeType);
        if (statement->isBuiltin())
        {
            if (statement->constructorParameters().has_value())
            {
                m_diagnostics.addBuiltinTypeConstructorIgnoredWarning(
                    tokens.source(),
                    statement->constructorParameters().value()->sourceLocation(tokens),
                    std::string(statement->name()));
            }
        }
        else
        {
            typeCheckConstructorSignature(statement, typeDefinition, typeType, tokens);
        }

        const auto& bodyStatements = statement->bodyNode()->statements();
        for (const auto& bodyStatement : bodyStatements)
        {
            if (bodyStatement->kind() != NodeKind::MethodDefinitionStatement)
            {
                continue;
            }

            const auto* methodStatement = static_cast<const MethodDefinitionStatement*>(bodyStatement.get());
            if (methodStatement->specialFunctionType() == SpecialFunctionType::Constructor || methodStatement->methodNameNode()->methodName() == "new")
            {
                continue;
            }

            if (methodStatement->type() == Type::Undefined())
            {
                continue;
            }

            typeCheckMethodSignature(methodStatement, typeDefinition, typeType, tokens);

            if (statement->isBuiltin())
            {
                validateBuiltinOperatorMethod(methodStatement, typeDefinition, typeType, tokens);
            }
        }
    }

    void TypeChecker::validateBuiltinOperatorMethod(const MethodDefinitionStatement* methodStatement, TypeDefinition& typeDefinition, Type typeType, const TokenBuffer& tokens)
    {
        const auto& methodName = methodStatement->methodNameNode()->methodName();
        const auto methodLocation = tokens.getSourceLocation(methodStatement->methodNameNode()->methodNameToken());

        const auto* operatorDefinition = TryGetBuiltinOperatorDefinition(methodName);
        if (operatorDefinition == nullptr)
        {
            m_diagnostics.addUnknownBuiltinMethodIgnoredWarning(
                tokens.source(),
                methodLocation,
                methodName,
                typeDefinition.name());
            return;
        }

        const auto methodType = typeDefinition.tryGetMethodTypeByName(methodName);
        if (methodType == Type::Undefined())
        {
            return;
        }

        const auto& methodDefinition = m_module.getFunctionDefinition(methodType);
        const auto& parameters = methodDefinition.parameters();
        const auto& returnTypes = methodDefinition.returnTypes();

        for (const auto& parameter : parameters)
        {
            if (parameter.type() == Type::Undefined())
            {
                return;
            }
        }

        const auto typeName = FormatTypeName(m_module, typeType);
        auto expectedSignature = std::string{};
        if (operatorDefinition->isUnary)
        {
            expectedSignature = typeName + "." + methodDefinition.name() + "(value: " + typeName + ")";
        }
        else
        {
            expectedSignature = typeName + "." + methodDefinition.name() + "(lhs: " + typeName + ", rhs: " + typeName + ")";
        }

        auto isValidSignature = returnTypes.size() == 1 && returnTypes.front() != Type::Void();
        if (isValidSignature)
        {
            isValidSignature = methodStatement->modifier() == MethodModifier::Static
                && methodStatement->methodNameNode()->hasTypeName()
                && methodStatement->methodNameNode()->typeName().value() == typeDefinition.name();
        }

        if (isValidSignature)
        {
            if (operatorDefinition->isUnary)
            {
                isValidSignature = parameters.size() == 1
                    && parameters[0].type() == typeType;
            }
            else
            {
                isValidSignature = parameters.size() == 2
                    && parameters[0].type() == typeType
                    && parameters[1].type() == typeType;
            }
        }

        if (!isValidSignature)
        {
            m_diagnostics.addInvalidOperatorMethodSignatureError(
                tokens.source(),
                methodLocation,
                methodName,
                expectedSignature);
            return;
        }

        if (operatorDefinition->isUnary)
        {
            typeDefinition.addOperatorSignature(
                operatorDefinition->unaryOperator,
                OperatorSignature{ parameters[0].type(), Type::Undefined(), returnTypes.front(), nullptr, operatorDefinition->unaryFold });
        }
        else
        {
            typeDefinition.addOperatorSignature(
                operatorDefinition->binaryOperator,
                OperatorSignature{ parameters[0].type(), parameters[1].type(), returnTypes.front(), operatorDefinition->binaryFold, nullptr });
        }
    }

    void TypeChecker::typeCheckTypeFieldDefinition(TypeDefinitionStatement* statement, const TokenBuffer& tokens)
    {
        auto typeType = statement->type();
        if (typeType == Type::Undefined())
        {
            TODO("This shouldn't happen");
        }

        if (statement->isBuiltin())
        {
            for (const auto& definitionStatement : statement->bodyNode()->statements())
            {
                if (definitionStatement->kind() != NodeKind::TypeFieldDeclaration)
                {
                    continue;
                }

                auto* fieldStatement = static_cast<TypeFieldDeclaration*>(definitionStatement.get());
                m_diagnostics.addBuiltinTypeFieldIgnoredWarning(
                    tokens.source(),
                    fieldStatement->nameExpression()->sourceLocation(tokens),
                    fieldStatement->nameExpression()->name());
            }

            return;
        }

        pushScope(ScopeKind::Type);

        if (statement->constructorParameters().has_value())
        {
            static_cast<void>(typeCheckParametersNode(statement->constructorParameters().value().get(), tokens));
        }

        auto& typeDefinition = m_module.getTypeDefinition(typeType);
        auto fieldIndex = 0;
        const auto& definitionStatements = statement->bodyNode()->statements();
        for (auto& definitionStatement : definitionStatements)
        {
            if (definitionStatement->kind() != NodeKind::TypeFieldDeclaration)
            {
                continue;
            }

            typeCheckTypeFieldDeclaration(typeDefinition, static_cast<TypeFieldDeclaration*>(definitionStatement.get()), fieldIndex, tokens);
            ++fieldIndex;
        }

        popScope(true);
    }

    void TypeChecker::typeCheckTypeMethodDefinition(TypeDefinitionStatement* statement, const TokenBuffer& tokens)
    {
        auto typeType = statement->type();
        if (typeType == Type::Undefined())
        {
            TODO("This shouldn't happen");
        }

        if (statement->isBuiltin())
        {
            for (const auto& bodyStatement : statement->bodyNode()->statements())
            {
                if (bodyStatement->kind() != NodeKind::MethodDefinitionStatement)
                {
                    continue;
                }

                auto* methodStatement = static_cast<MethodDefinitionStatement*>(bodyStatement.get());
                if (!methodStatement->bodyNode()->statements().empty())
                {
                    m_diagnostics.addBuiltinMethodBodyIgnoredWarning(
                        tokens.source(),
                        tokens.getSourceLocation(methodStatement->methodNameNode()->methodNameToken()),
                        methodStatement->methodNameNode()->methodName());
                }
            }

            return;
        }

        m_currentType = typeType;
        pushScope(ScopeKind::Type);

        if (statement->constructorParameters().has_value())
        {
            static_cast<void>(typeCheckParametersNode(statement->constructorParameters().value().get(), tokens));
        }

        auto& typeDefinition = m_module.getTypeDefinition(typeType);
        for (const auto& fieldDefinition : typeDefinition.fields())
        {
            currentScope()->addVariableBinding(
                fieldDefinition.name(),
                fieldDefinition.type(),
                GetTypeFieldLocation(typeDefinition, fieldDefinition, tokens),
                tokens.source(),
                VariableBindingKind::Field);
        }

        const auto& definitionStatements = statement->bodyNode()->statements();
        for (auto& definitionStatement : definitionStatements)
        {
            if (definitionStatement->kind() != NodeKind::MethodDefinitionStatement)
            {
                continue;
            }

            auto* methodStatement = static_cast<MethodDefinitionStatement*>(definitionStatement.get());
            if (methodStatement->specialFunctionType() == SpecialFunctionType::Constructor || methodStatement->methodNameNode()->methodName() == "new")
            {
                continue;
            }

            if (methodStatement->type() == Type::Undefined())
            {
                continue;
            }

            typeCheckMethodDefinitionStatement(methodStatement, tokens);
        }

        popScope(false);
        m_currentType = Type::Undefined();
    }

    void TypeChecker::typeCheckMethodSignature(const MethodDefinitionStatement* methodStatement, TypeDefinition& typeDefinition, Type typeType, const TokenBuffer& tokens)
    {
        const auto& methodName = methodStatement->methodNameNode()->methodName();

        pushScope(ScopeKind::Method);
        auto parameters = typeCheckParametersNode(methodStatement->parametersNode().get(), tokens);
        auto returns = typeCheckReturnTypesNode(methodStatement->returnTypesNode().get(), tokens);
        popScope(false);

        auto methodType = typeDefinition.tryGetMethodTypeByName(methodName);
        if (methodType == Type::Undefined())
        {
            TODO("This shouldn't happen");
        }

        auto& methodDefinition = m_module.getFunctionDefinition(methodType);

        std::optional<std::string> symbolName;
        const auto isExtern = validateCallableAnnotations(methodStatement->annotations(), tokens, symbolName);
        if (isExtern && !symbolName.has_value())
        {
            // methods need to specify the extern symbol name
            for (const auto& annotation : methodStatement->annotations())
            {
                if (annotation->kind() == AnnotationKind::Extern)
                {
                    m_diagnostics.addExternMethodRequiresSymbolError(tokens.source(), annotation->sourceLocation(tokens), methodDefinition.fullName());
                    break;
                }
            }
        }
        const auto& parameterNodes = methodStatement->parametersNode()->parameters();
        const auto isVariadic = !parameterNodes.empty() && parameterNodes.back()->isVariadic();
        if (isVariadic && !isExtern)
        {
            auto* variadicParameter = parameterNodes.back().get();
            m_diagnostics.addNonExternVariadicFunctionError(
                tokens.source(),
                variadicParameter->sourceLocation(tokens),
                methodName);
        }

        if (methodStatement->modifier() != MethodModifier::Static)
        {
            parameters.insert(parameters.begin(), Parameter{ ImplicitThisName, typeType.toReference() });
        }
        methodDefinition.setParameters(parameters);
        methodDefinition.setReturnTypes(returns);
        methodDefinition.setIsVariadic(isVariadic);
        methodDefinition.setSymbolName(std::move(symbolName));
    }

    void TypeChecker::typeCheckConstructorSignature(const TypeDefinitionStatement* typeDefinitionStatement, TypeDefinition& typeDefinition, Type typeType, const TokenBuffer& tokens)
    {
        auto constructorType = typeDefinition.tryGetMethodTypeByName("new");
        if (constructorType == Type::Undefined())
        {
            TODO("This shouldn't happen");
        }

        std::vector<Parameter> constructorParameters{};
        constructorParameters.emplace_back(ImplicitThisName, typeType.toReference());

        if (typeDefinitionStatement->constructorParameters().has_value())
        {
            pushScope(ScopeKind::Type);
            auto declaredConstructorParameters = typeCheckParametersNode(typeDefinitionStatement->constructorParameters().value().get(), tokens);
            popScope(false);

            constructorParameters.insert(
                constructorParameters.end(),
                declaredConstructorParameters.begin(),
                declaredConstructorParameters.end());
        }

        auto& constructorDefinition = m_module.getFunctionDefinition(constructorType);
        constructorDefinition.setParameters(constructorParameters);
        constructorDefinition.setReturnTypes(std::vector<Type>{});
    }

    void TypeChecker::typeCheckStatement(Statement* statement, const TokenBuffer& tokens)
    {
        switch (statement->kind())
        {
            case NodeKind::ConstantDeclaration:
            {
                typeCheckConstantDeclaration(static_cast<ConstantDeclaration*>(statement), tokens);
                break;
            }
            case NodeKind::VariableDeclaration:
            {
                typeCheckVariableDeclaration(static_cast<VariableDeclaration*>(statement), tokens);
                break;
            }
            case NodeKind::ExpressionStatement:
            {
                typeCheckExpressionStatement(static_cast<ExpressionStatement*>(statement), tokens);
                break;
            }
            case NodeKind::AssignmentStatement:
            {
                typeCheckAssignmentStatement(static_cast<AssignmentStatement*>(statement), tokens);
                break;
            }
            case NodeKind::FunctionDefinitionStatement:
            {
                typeCheckFunctionDefinitionStatement(static_cast<FunctionDefinitionStatement*>(statement), tokens);
                break;
            }
            case NodeKind::EnumDefinitionStatement:
            {
                typeCheckEnumDefinitionStatement(static_cast<EnumDefinitionStatement*>(statement), tokens);
                break;
            }
            case NodeKind::IfStatement:
            {
                typeCheckIfStatement(static_cast<IfStatement*>(statement), tokens);
                break;
            }
            case NodeKind::WhileStatement:
            {
                typeCheckWhileStatement(static_cast<WhileStatement*>(statement), tokens);
                break;
            }
            case NodeKind::ReturnStatement:
            {
                typeCheckReturnStatement(static_cast<ReturnStatement*>(statement), tokens);
                break;
            }
            case NodeKind::BlockNode:
            {
                typeCheckBlockNode(static_cast<BlockNode*>(statement), tokens);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    void TypeChecker::typeCheckConstantDeclaration(ConstantDeclaration* statement, const TokenBuffer& tokens)
    {
        if (statement->isGlobalConstant())
        {
            for (const auto& annotation : statement->annotations())
            {
                m_diagnostics.addUnexpectedAnnotationTargetError(
                    tokens.source(),
                    annotation->sourceLocation(tokens));
            }
        }

        if (statement->isInit())
        {
            auto declaredType = Type::Undefined();
            if (statement->explicitType().has_value())
            {
                declaredType = typeCheckTypeNameNode(statement->explicitType().value().get(), tokens);
            }

            const auto isValidInitConstant = statement->isGlobalConstant();
            auto* leftExpression = statement->leftExpression().get();
            if (leftExpression->kind() == NodeKind::NameExpression)
            {
                auto* nameExpression = static_cast<NameExpression*>(leftExpression);
                const auto& name = nameExpression->name();

                if (!isValidInitConstant)
                {
                    m_diagnostics.addNonGlobalInitConstantError(
                        tokens.source(),
                        nameExpression->sourceLocation(tokens),
                        name);
                }

                auto scope = currentScope();
                if (!scope->hasVariableBinding(name))
                {
                    scope->addVariableBinding(name, declaredType, nameExpression->sourceLocation(tokens), tokens.source(), VariableBindingKind::LocalConstant);

                    if (isValidInitConstant)
                    {
                        static_cast<void>(m_module.createInitConstant(name, declaredType));
                        m_initConstants.insert_or_assign(name, InitConstantDeclaration{ nameExpression->sourceLocation(tokens), tokens.source() });
                    }
                }
                else
                {
                    m_diagnostics.addDuplicateConstantDeclarationError(
                        tokens.source(),
                        nameExpression->sourceLocation(tokens),
                        name,
                        scope->tryGetVariableBindingSource(name),
                        scope->tryGetVariableBindingLocation(name));
                }
                nameExpression->setType(declaredType);
            }

            statement->setType(declaredType);
            return;
        }

        auto rightExpression = statement->rightExpression().get();
        auto rightType = typeCheckExpression(rightExpression, tokens);

        auto leftExpression = statement->leftExpression().get();
        if (leftExpression->kind() == NodeKind::NameExpression)
        {
            auto nameExpression = static_cast<NameExpression*>(leftExpression);
            const auto& name = nameExpression->name();
            auto scope = currentScope();
            if (!scope->hasVariableBinding(name))
            {
                scope->addVariableBinding(name, rightType, nameExpression->sourceLocation(tokens), tokens.source(), VariableBindingKind::LocalConstant);

                if (statement->isGlobalConstant())
                {
                    // TODO maybe remove the return value from the function?
                    static_cast<void>(m_module.createConstant(name, rightExpression));
                }
            }
            else
            {
                m_diagnostics.addDuplicateConstantDeclarationError(
                    tokens.source(),
                    nameExpression->sourceLocation(tokens),
                    name,
                    scope->tryGetVariableBindingSource(name),
                    scope->tryGetVariableBindingLocation(name));
            }
            nameExpression->setType(rightType);
        }
        else if (statement->isGlobalConstant() && leftExpression->kind() == NodeKind::DiscardLiteral)
        {
            // a discarded global still needs to evaluate for potential side effects
            m_module.addGlobalDiscardExpression(rightExpression);
        }

        if (statement->explicitType().has_value())
        {
            auto explicitType = typeCheckTypeNameNode(statement->explicitType().value().get(), tokens);
            if (rightType != Type::Undefined() && explicitType != Type::Undefined() && rightType != explicitType)
            {
                m_diagnostics.addExplicitConstantTypeMismatchError(
                    tokens.source(),
                    statement->rightExpression()->sourceLocation(tokens),
                    FormatTypeName(m_module, explicitType),
                    FormatTypeName(m_module, rightType));
            }
        }

        statement->setType(rightType);
    }

    void TypeChecker::typeCheckVariableDeclaration(VariableDeclaration* statement, const TokenBuffer& tokens)
    {
        auto* rightExpression = statement->rightExpression().get();
        auto rightType = typeCheckExpression(rightExpression, tokens);

        auto referencesConstant = false;
        if (rightExpression->kind() == NodeKind::UnaryExpression)
        {
            const auto* unary = static_cast<const UnaryExpression*>(rightExpression);
            referencesConstant = unary->unaryOperator() == UnaryOperatorKind::ReferenceOf && unary->referencesConstant();
        }
        else if (rightExpression->kind() == NodeKind::NameExpression)
        {
            referencesConstant = currentScope()->variableReferencesConstant(static_cast<const NameExpression*>(rightExpression)->name());
        }

        auto leftExpression = statement->leftExpression().get();
        if (leftExpression->kind() == NodeKind::NameExpression)
        {
            auto nameExpression = static_cast<NameExpression*>(leftExpression);
            const auto& name = nameExpression->name();
            auto scope = currentScope();
            if (!scope->hasVariableBinding(name))
            {
                scope->addVariableBinding(name, rightType, nameExpression->sourceLocation(tokens), tokens.source(), VariableBindingKind::LocalVariable, referencesConstant);
            }
            else
            {
                m_diagnostics.addDuplicateVariableDeclarationError(
                    tokens.source(),
                    nameExpression->sourceLocation(tokens),
                    name,
                    scope->tryGetVariableBindingSource(name),
                    scope->tryGetVariableBindingLocation(name));
            }
            nameExpression->setType(rightType);
        }

        if (statement->explicitType().has_value())
        {
            auto explicitType = typeCheckTypeNameNode(statement->explicitType().value().get(), tokens);
            if (rightType != Type::Undefined() && explicitType != Type::Undefined() && rightType != explicitType)
            {
                m_diagnostics.addExplicitVariableTypeMismatchError(
                    tokens.source(),
                    statement->rightExpression()->sourceLocation(tokens),
                    FormatTypeName(m_module, explicitType),
                    FormatTypeName(m_module, rightType));
            }
        }

        statement->setType(rightType);
    }

    void TypeChecker::typeCheckExpressionStatement(ExpressionStatement* statement, const TokenBuffer& tokens)
    {
        auto expressionType = typeCheckExpression(statement->expression().get(), tokens);
        statement->setType(expressionType);
    }

    void TypeChecker::typeCheckAssignmentStatement(AssignmentStatement* statement, const TokenBuffer& tokens)
    {
        auto* leftExpression = statement->leftExpression().get();
        auto leftType = typeCheckExpression(leftExpression, tokens);
        if (leftType.isReference())
        {
            leftType = leftType.toValue();
        }

        // global init constants are only allowed to be assigned once in caracalMain/main 
        if (leftExpression->kind() == NodeKind::NameExpression)
        {
            const auto& targetName = static_cast<const NameExpression*>(leftExpression)->name();
            if (m_initConstants.contains(targetName))
            {
                const auto location = leftExpression->sourceLocation(tokens);
                if (!IsInitConstantAssignmentSite(m_currentFunctionName))
                {
                    m_diagnostics.addAssignmentToInitConstantError(tokens.source(), location, targetName);
                }
                else if (const auto existing = m_initConstantAssignments.find(targetName); existing != m_initConstantAssignments.end())
                {
                    m_diagnostics.addInitConstantAlreadyInitializedError(tokens.source(), location, targetName, existing->second);
                }
                else
                {
                    m_initConstantAssignments.emplace(targetName, location);
                }
            }
            else if (currentScope()->tryGetVariableBindingKind(targetName) == VariableBindingKind::LocalConstant)
            {
                m_diagnostics.addAssignmentToConstantError(
                    tokens.source(),
                    leftExpression->sourceLocation(tokens),
                    targetName);
            }
            else if (currentScope()->variableReferencesConstant(targetName))
            {
                m_diagnostics.addAssignmentThroughConstantReferenceError(
                    tokens.source(),
                    leftExpression->sourceLocation(tokens),
                    targetName);
            }
        }
        else if (leftExpression->kind() == NodeKind::BinaryExpression)
        {
            const auto* memberAccess = static_cast<const BinaryExpression*>(leftExpression);
            if (memberAccess->binaryOperator() == BinaryOperatorKind::MemberAccess &&
                memberAccess->rightExpression()->kind() == NodeKind::NameExpression)
            {
                auto receiverType = memberAccess->leftExpression()->type();
                if (receiverType.isReference())
                {
                    receiverType = receiverType.toValue();
                }

                if (receiverType.kind() == TypeKind::Type)
                {
                    auto& typeDefinition = m_module.getTypeDefinition(receiverType);
                    const auto& fieldName = static_cast<const NameExpression*>(memberAccess->rightExpression().get())->name();
                    const auto& fieldDefinition = typeDefinition.tryGetFieldByName(fieldName);
                    if (fieldDefinition.type() != Type::Undefined() && fieldDefinition.isConstant())
                    {
                        m_diagnostics.addAssignmentToConstantError(
                            tokens.source(),
                            leftExpression->sourceLocation(tokens),
                            fieldName);
                    }
                }
            }
        }
        auto rightType = typeCheckExpression(statement->rightExpression().get(), tokens);
        if (rightType.isReference())
        {
            rightType = rightType.toValue();
        }

        if (leftType == Type::Discard())
            return;

        if (leftType != rightType)
        {
            m_diagnostics.addAssignmentTypeMismatchError(
                tokens.source(),
                statement->rightExpression()->sourceLocation(tokens),
                FormatTypeName(m_module, leftType),
                FormatTypeName(m_module, rightType));
        }
    }

    static bool ConditionIsLiteralTrue(const Expression* condition)
    {
        while (condition->kind() == NodeKind::GroupingExpression)
            condition = static_cast<const GroupingExpression*>(condition)->expression().get();

        return condition->kind() == NodeKind::BoolLiteral && static_cast<const BoolLiteral*>(condition)->value();
    }

    static bool ContainsLoopBreak(const Statement* statement)
    {
        switch (statement->kind())
        {
            case NodeKind::BreakStatement:
            {
                return true;
            }
            case NodeKind::BlockNode:
            {
                for (const auto& inner : static_cast<const BlockNode*>(statement)->statements())
                {
                    if (ContainsLoopBreak(inner.get()))
                    {
                        return true;
                    }
                }
                return false;
            }
            case NodeKind::IfStatement:
            {
                const auto* ifStatement = static_cast<const IfStatement*>(statement);
                if (ContainsLoopBreak(ifStatement->trueStatement().get()))
                {
                    return true;
                }

                return ifStatement->falseStatement().has_value() && ContainsLoopBreak(ifStatement->falseStatement().value().get());
            }
            default:
                return false;
        }
    }

    static bool StatementGuaranteesReturn(const Statement* statement)
    {
        switch (statement->kind())
        {
            case NodeKind::ReturnStatement:
            {
                return true;
            }
            case NodeKind::BlockNode:
            {
                for (const auto& inner : static_cast<const BlockNode*>(statement)->statements())
                {
                    if (StatementGuaranteesReturn(inner.get()))
                    {
                        return true;
                    }
                }
                return false;
            }
            case NodeKind::IfStatement:
            {
                const auto* ifStatement = static_cast<const IfStatement*>(statement);
                return ifStatement->falseStatement().has_value()
                    && StatementGuaranteesReturn(ifStatement->trueStatement().get())
                    && StatementGuaranteesReturn(ifStatement->falseStatement().value().get());
            }
            case NodeKind::WhileStatement:
            {
                const auto* whileStatement = static_cast<const WhileStatement*>(statement);
                return ConditionIsLiteralTrue(whileStatement->condition().get())
                    && !ContainsLoopBreak(whileStatement->trueStatement().get());
            }
            default:
                return false;
        }
    }

    static bool HasExternAnnotation(const std::vector<AnnotationNodeUPtr>& annotations)
    {
        for (const auto& annotation : annotations)
        {
            if (annotation->kind() == AnnotationKind::Extern)
            {
                return true;
            }
        }

        return false;
    }

    void TypeChecker::typeCheckFunctionDefinitionStatement(FunctionDefinitionStatement* statement, const TokenBuffer& tokens)
    {
        m_currentReturnType = Type::Void();
        auto parentScope = currentScope();
        pushScope(ScopeKind::Function);

        auto functionType = statement->type();
        if (functionType == Type::Undefined())
        {
            TODO("This shouldn't happen");
        }

        auto& functionDefinition = m_module.getFunctionDefinition(functionType);
        auto& parameters = functionDefinition.parameters();
        const auto& parameterNodes = statement->parametersNode()->parameters();
        for (size_t i = 0; i < parameters.size(); ++i)
        {
            auto location = std::optional<SourceLocation>{};
            if (i < parameterNodes.size())
            {
                location = parameterNodes[i]->sourceLocation(tokens);
            }
            currentScope()->addVariableBinding(parameters[i].name(), parameters[i].type(), location, tokens.source(), VariableBindingKind::Parameter);
        }

        auto& returnTypes = functionDefinition.returnTypes();
        if (returnTypes.size() == 1)
        {
            m_currentReturnType = returnTypes[0];
        }
        else if (returnTypes.size() > 1)
        {
            TODO("Handle multiple return types in function definition");
        }

        m_currentFunctionName = functionDefinition.fullName();
        typeCheckBlockNode(statement->bodyNode().get(), tokens);
        m_currentFunctionName.clear();

        if (!statement->isExtern()
            && m_currentReturnType != Type::Void()
            && m_currentReturnType != Type::Undefined()
            && !StatementGuaranteesReturn(statement->bodyNode().get()))
        {
            m_diagnostics.addMissingReturnError(
                tokens.source(),
                tokens.getSourceLocation(statement->nameToken()),
                functionDefinition.name(),
                FormatTypeName(m_module, m_currentReturnType));
        }

        popScope(!statement->isExtern());
        m_currentReturnType = Type::Void();
    }

    void TypeChecker::typeCheckEnumDefinitionStatement(EnumDefinitionStatement* statement, const TokenBuffer& tokens)
    {
        const auto& enumName = statement->name();
        auto baseType = Type::Undefined();
        auto defaultBaseType = m_options.defaultEnumBaseType;
        auto enumType = statement->type();
        if (enumType == Type::Undefined())
        {
            TODO("This shouldn't happen");
        }

        auto& enumDefinition = m_module.getEnumDefinition(enumType);
        auto isFlag = false;
        auto stepValue = std::optional<i32>{};
        validateEnumAnnotation(statement, tokens, isFlag, stepValue);
        auto currentFieldValue = isFlag ? 1 : 0;
        auto step = stepValue.value_or(1);

        if (statement->baseType().has_value())
        {
            baseType = typeCheckTypeNameNode(statement->baseType().value().get(), tokens);
            enumDefinition.setBaseType(baseType);
        }

        const auto& fieldNodes = statement->fieldNodes();
        for (auto& fieldNode : fieldNodes)
        {
            const auto& fieldName = fieldNode->name();
            if (enumDefinition.hasField(fieldName))
            {
                const auto& otherField = enumDefinition.getFieldByName(fieldName);
                m_diagnostics.addDuplicateEnumFieldDeclarationError(
                    tokens.source(),
                    tokens.getSourceLocation(fieldNode->nameToken()),
                    fieldName,
                    otherField.location());
                continue;
            }

            const auto fieldLocation = tokens.getSourceLocation(fieldNode->nameToken());
            if (isFlag)
            {
                if (fieldNode->valueExpression().has_value())
                {
                    m_diagnostics.addFlagEnumExplicitValueError(
                        tokens.source(),
                        fieldNode->valueExpression().value()->sourceLocation(tokens),
                        fieldName);
                }

                enumDefinition.addField(fieldName, currentFieldValue, fieldLocation);
                fieldNode->setValue(currentFieldValue);

                if (currentFieldValue == 0)
                {
                    currentFieldValue = 1;
                }
                else
                {
                    currentFieldValue *= 2;
                }
            }
            else if (fieldNode->valueExpression().has_value())
            {
                auto expression = fieldNode->valueExpression().value().get();
                m_contextualNumberType = baseType == Type::Undefined() ? defaultBaseType : baseType;
                auto fieldValueType = typeCheckExpression(expression, tokens);
                if (baseType == Type::Undefined())
                {
                    baseType = fieldValueType;
                    enumDefinition.setBaseType(baseType);
                }
                else if (fieldValueType != baseType)
                {
                    if (fieldValueType != Type::Undefined())
                    {
                        m_diagnostics.addEnumFieldValueTypeMismatchError(
                            tokens.source(),
                            expression->sourceLocation(tokens),
                            FormatTypeName(m_module, baseType),
                            FormatTypeName(m_module, fieldValueType));
                    }
                }

                if (expression->kind() == NodeKind::NumberLiteral)
                {
                    const auto value = TryConvertEnumFieldLiteralValue(*static_cast<NumberLiteral*>(expression));
                    if (value.has_value())
                    {
                        enumDefinition.addField(fieldName, value.value(), fieldLocation);
                        fieldNode->setValue(value.value());
                        currentFieldValue = value.value() + step;
                    }
                    else
                    {
                        enumDefinition.addField(fieldName, expression, fieldLocation);
                    }
                }
                else
                {
                    enumDefinition.addField(fieldName, expression, fieldLocation);
                }
            }
            else
            {
                enumDefinition.addField(fieldName, currentFieldValue, fieldLocation);
                fieldNode->setValue(currentFieldValue);
                currentFieldValue += step;
            }
        }

        if (enumDefinition.baseType() == Type::Undefined())
        {
            enumDefinition.setBaseType(defaultBaseType);
        }
    }

    bool TypeChecker::validateNamedAnnotationArguments(const AnnotationNode* annotation, std::string_view namedStringArgument, const TokenBuffer& tokens, std::optional<std::string>* stringArgumentValue)
    {
        std::vector<std::string_view> seenArgumentNames;
        for (const auto& argument : annotation->arguments())
        {
            if (!argument.isNamed())
            {
                continue;
            }

            const auto nameLocation = tokens.getSourceLocation(argument.nameToken().value());
            if (std::find(seenArgumentNames.begin(), seenArgumentNames.end(), argument.name()) != seenArgumentNames.end())
            {
                m_diagnostics.addDuplicateAnnotationArgumentError(tokens.source(), nameLocation, annotation->name(), argument.name());
                return false;
            }
            seenArgumentNames.push_back(argument.name());

            if (namedStringArgument.empty() || argument.name() != namedStringArgument)
            {
                m_diagnostics.addUnexpectedAnnotationArgumentError(tokens.source(), nameLocation, annotation->name(), argument.name());
                return false;
            }

            auto* value = argument.value().get();
            const auto argumentType = typeCheckExpression(value, tokens);
            if (argumentType == Type::Undefined())
            {
                return false;
            }

            if (value->kind() != NodeKind::StringLiteral)
            {
                m_diagnostics.addAnnotationArgumentTypeMismatchError(
                    tokens.source(),
                    value->sourceLocation(tokens),
                    annotation->kind(),
                    annotation->name(),
                    "a string literal for '" + std::string(namedStringArgument) + "'",
                    "an expression of type '" + FormatTypeName(m_module, argumentType) + "'");
                return false;
            }

            if (stringArgumentValue != nullptr)
            {
                *stringArgumentValue = static_cast<const StringLiteral*>(value)->escapedContent();
            }
        }

        return true;
    }

    bool TypeChecker::validateAnnotation(const AnnotationNode* annotation, TokenKind targetKind, const TokenBuffer& tokens, std::optional<i32>* i32ArgumentValue, std::optional<std::string>* stringArgumentValue)
    {
        const auto annotationLocation = annotation->sourceLocation(tokens);
        const auto* definition = GetAnnotationDefinition(annotation->kind());
        if (definition == nullptr)
        {
            m_diagnostics.addUnknownAnnotationError(tokens.source(), annotationLocation, annotation->name(), targetKind);
            return false;
        }

        if (definition->targetKind != targetKind)
        {
            m_diagnostics.addUnexpectedAnnotationTargetError(tokens.source(), annotationLocation);
            return false;
        }

        if (!validateNamedAnnotationArguments(annotation, definition->namedStringArgument, tokens, stringArgumentValue))
        {
            return false;
        }

        Expression* firstPositionalArgument = nullptr;
        i32 actualCount = 0;
        for (const auto& argument : annotation->arguments())
        {
            if (argument.isNamed())
            {
                continue;
            }

            if (firstPositionalArgument == nullptr)
            {
                firstPositionalArgument = argument.value().get();
            }
            ++actualCount;
        }

        if (!definition->namedStringArgument.empty() && actualCount > 0
            && firstPositionalArgument->kind() == NodeKind::StringLiteral)
        {
            m_diagnostics.addAnnotationMissingArgumentsError(
                tokens.source(),
                annotation->argumentsLocation(tokens),
                annotation->kind(),
                annotation->name(),
                std::string(definition->namedStringArgument));
            return false;
        }

        if (definition->requiredArgumentCount > 0 && !annotation->hasParentheses())
        {
            m_diagnostics.addAnnotationMissingArgumentsError(tokens.source(), annotationLocation, annotation->kind(), annotation->name());
            return false;
        }

        if (actualCount != definition->requiredArgumentCount)
        {
            m_diagnostics.addAnnotationWrongNumberOfArgumentsError(
                tokens.source(),
                annotation->argumentsLocation(tokens),
                annotation->kind(),
                annotation->name(),
                definition->requiredArgumentCount,
                actualCount);
            return false;
        }

        if (definition->parameterType != Type::Undefined())
        {
            auto* argument = firstPositionalArgument;
            auto argumentType = typeCheckExpression(argument, tokens);
            if (argumentType == Type::Undefined())
            {
                return false;
            }

            if (argument->kind() != NodeKind::NumberLiteral)
            {
                m_diagnostics.addAnnotationArgumentTypeMismatchError(
                    tokens.source(),
                    argument->sourceLocation(tokens),
                    annotation->kind(),
                    annotation->name(),
                    "a " + FormatTypeName(m_module, definition->parameterType) + " number literal argument",
                    "an expression of type '" + FormatTypeName(m_module, argumentType) + "'");
                return false;
            }

            if (argumentType != definition->parameterType)
            {
                m_diagnostics.addAnnotationArgumentTypeMismatchError(
                    tokens.source(),
                    argument->sourceLocation(tokens),
                    annotation->kind(),
                    annotation->name(),
                    "a " + FormatTypeName(m_module, definition->parameterType) + " number literal argument",
                    "a number literal of type '" + FormatTypeName(m_module, argumentType) + "'");
                return false;
            }

            if (definition->parameterType == Type::I32() && i32ArgumentValue != nullptr)
            {
                *i32ArgumentValue = convertToI32(static_cast<NumberLiteral*>(argument), tokens);
            }
        }

        return true;
    }

    bool TypeChecker::validateCallableAnnotations(const std::vector<AnnotationNodeUPtr>& annotations, const TokenBuffer& tokens, std::optional<std::string>& symbolName)
    {
        auto isExtern = false;
        for (const auto& annotationNode : annotations)
        {
            const auto* annotation = annotationNode.get();
            std::optional<std::string> annotationSymbolName;
            if (!validateAnnotation(annotation, TokenKind::DefKeyword, tokens, nullptr, &annotationSymbolName))
            {
                continue;
            }

            if (annotation->kind() == AnnotationKind::Extern)
            {
                isExtern = true;
                if (annotationSymbolName.has_value())
                {
                    symbolName = std::move(annotationSymbolName);
                }
            }
        }

        return isExtern;
    }

    void TypeChecker::validateEnumAnnotation(const EnumDefinitionStatement* statement, const TokenBuffer& tokens, bool& isFlag, std::optional<i32>& stepValue)
    {
        isFlag = false;
        stepValue = std::nullopt;

        const AnnotationNode* flagAnnotation = nullptr;
        const AnnotationNode* stepAnnotation = nullptr;

        for (const auto& annotationNode : statement->annotations())
        {
            const auto* annotation = annotationNode.get();
            auto i32ArgumentValue = std::optional<i32>{};
            if (!validateAnnotation(annotation, TokenKind::EnumKeyword, tokens, &i32ArgumentValue))
            {
                continue;
            }

            switch (annotation->kind())
            {
                case AnnotationKind::Flag:
                    if (stepAnnotation != nullptr)
                    {
                        m_diagnostics.addConflictingEnumAnnotationsError(
                            tokens.source(),
                            annotation->sourceLocation(tokens),
                            stepAnnotation->sourceLocation(tokens),
                            annotation->name(),
                            stepAnnotation->name());
                        break;
                    }

                    isFlag = true;
                    flagAnnotation = annotation;
                    break;
                case AnnotationKind::Step:
                    if (flagAnnotation != nullptr)
                    {
                        m_diagnostics.addConflictingEnumAnnotationsError(
                            tokens.source(),
                            annotation->sourceLocation(tokens),
                            flagAnnotation->sourceLocation(tokens),
                            annotation->name(),
                            flagAnnotation->name());
                        break;
                    }

                    stepValue = i32ArgumentValue;
                    stepAnnotation = annotation;
                    break;
                default:
                    break;
            }
        }
    }

    void TypeChecker::validateTypeAnnotation(const TypeDefinitionStatement* statement, const TokenBuffer& tokens)
    {
        for (const auto& annotationNode : statement->annotations())
        {
            static_cast<void>(validateAnnotation(annotationNode.get(), TokenKind::TypeKeyword, tokens));
        }
    }

    void TypeChecker::typeCheckTypeFieldDeclaration(TypeDefinition& typeDefinition, TypeFieldDeclaration* statement, i32 fieldIndex, const TokenBuffer& tokens)
    {
        const auto& fieldName = statement->nameExpression()->name();
        const auto& existingField = typeDefinition.tryGetFieldByName(fieldName);
        if (existingField.type() != Type::Undefined())
        {
            m_diagnostics.addDuplicateTypeFieldDeclarationError(
                tokens.source(),
                statement->nameExpression()->sourceLocation(tokens),
                fieldName,
                GetTypeFieldLocation(typeDefinition, existingField, tokens));
            return;
        }

        auto fieldType = Type::Undefined();
        if (statement->explicitType().has_value())
        {
            fieldType = typeCheckTypeNameNode(statement->explicitType().value().get(), tokens);
        }

        auto fieldExpression = statement->rightExpression().get();
        auto expressionType = typeCheckExpression(fieldExpression, tokens);
        if (fieldType == Type::Undefined())
        {
            fieldType = expressionType;
        }
        else if (fieldType != expressionType)
        {
            m_diagnostics.addTypeFieldInitializerMismatchError(
                tokens.source(),
                fieldExpression->sourceLocation(tokens),
                FormatTypeName(m_module, fieldType),
                FormatTypeName(m_module, expressionType));
        }

        if (fieldType == Type::Undefined())
        {
            return;
        }

        statement->nameExpression()->setType(fieldType);
        statement->setType(fieldType);
        typeDefinition.addField(fieldType, fieldName, fieldIndex, fieldExpression, statement->isConstant());
    }

    void TypeChecker::typeCheckMethodDefinitionStatement(MethodDefinitionStatement* statement, const TokenBuffer& tokens)
    {
        m_currentReturnType = Type::Void();
        auto methodType = statement->type();
        if (methodType == Type::Undefined())
        {
            TODO("This shouldn't happen");
        }

        pushScope(ScopeKind::Method);

        auto& methodDefinition = m_module.getFunctionDefinition(methodType);
        auto& parameters = methodDefinition.parameters();
        const auto& parameterNodes = statement->parametersNode()->parameters();
        for (size_t i = 0; i < parameters.size(); ++i)
        {
            auto location = std::optional<SourceLocation>{};
            if (i < parameterNodes.size())
            {
                location = parameterNodes[i]->sourceLocation(tokens);
            }
            currentScope()->addVariableBinding(parameters[i].name(), parameters[i].type(), location, tokens.source(), VariableBindingKind::Parameter);
        }

        auto& returnTypes = methodDefinition.returnTypes();
        if (returnTypes.size() == 1)
        {
            m_currentReturnType = returnTypes[0];
        }
        else if (returnTypes.size() > 1)
        {
            TODO("Handle multiple return types in function definition");
        }

        m_currentFunctionName.clear();
        typeCheckBlockNode(statement->bodyNode().get(), tokens);

        if (!HasExternAnnotation(statement->annotations())
            && m_currentReturnType != Type::Void()
            && m_currentReturnType != Type::Undefined()
            && !StatementGuaranteesReturn(statement->bodyNode().get()))
        {
            m_diagnostics.addMissingReturnError(
                tokens.source(),
                tokens.getSourceLocation(statement->methodNameNode()->methodNameToken()),
                methodDefinition.name(),
                FormatTypeName(m_module, m_currentReturnType));
        }

        popScope(true);
        m_currentReturnType = Type::Void();
    }

    void TypeChecker::typeCheckIfStatement(IfStatement* statement, const TokenBuffer& tokens)
    {
        auto conditionType = typeCheckExpression(statement->condition().get(), tokens);
        conditionType = coerceConditionType(conditionType, statement->condition().get());
        if (conditionType != Type::Undefined() && conditionType != Type::Bool())
        {
            m_diagnostics.addNonBoolIfConditionError(
                tokens.source(),
                statement->condition()->sourceLocation(tokens),
                FormatTypeName(m_module, conditionType));
        }

        typeCheckStatement(statement->trueStatement().get(), tokens);

        if (statement->hasFalseBlock())
        {
            typeCheckStatement(statement->falseStatement().value().get(), tokens);
        }
    }

    void TypeChecker::typeCheckWhileStatement(WhileStatement* statement, const TokenBuffer& tokens)
    {
        auto conditionType = typeCheckExpression(statement->condition().get(), tokens);
        conditionType = coerceConditionType(conditionType, statement->condition().get());
        if (conditionType != Type::Undefined() && conditionType != Type::Bool())
        {
            m_diagnostics.addNonBoolWhileConditionError(
                tokens.source(),
                statement->condition()->sourceLocation(tokens),
                FormatTypeName(m_module, conditionType));
        }

        typeCheckStatement(statement->trueStatement().get(), tokens);
    }

    void TypeChecker::typeCheckReturnStatement(ReturnStatement* statement, const TokenBuffer& tokens)
    {
        const auto declaredReturnType = m_currentReturnType;

        if (statement->expression().has_value())
        {
            auto type = typeCheckExpression(statement->expression().value().get(), tokens);
            if (type.isReference())
            {
                // returning a ref is now allowed, so we need to coerce it to a value
                type = type.toValue();
            }

            statement->setType(type);
            if (type == Type::Undefined())
            {
                return;
            }

            if (declaredReturnType != type)
            {
                m_diagnostics.addReturnTypeMismatchError(
                    tokens.source(),
                    statement->expression().value()->sourceLocation(tokens),
                    FormatTypeName(m_module, declaredReturnType),
                    FormatTypeName(m_module, type));
            }
        }
        else
        {
            statement->setType(Type::Void());

            if (declaredReturnType != Type::Void())
            {
                const auto returnKeywordLocation = tokens.getSourceLocation(statement->keywordToken());
                const auto semicolonLocation = tokens.getSourceLocation(statement->semicolonToken());
                m_diagnostics.addReturnTypeMismatchError(
                    tokens.source(),
                    SourceLocation{ returnKeywordLocation.startIndex, semicolonLocation.endIndex },
                    FormatTypeName(m_module, declaredReturnType),
                    FormatTypeName(m_module, Type::Void()));
            }
        }
    }

    Type TypeChecker::typeCheckExpression(Expression* expression, const TokenBuffer& tokens)
    {
        switch (expression->kind())
        {
            case NodeKind::StringLiteral:
            case NodeKind::BoolLiteral:
            {
                return expression->type();
            }
            case NodeKind::NumberLiteral:
            {
                return typeCheckNumberLiteral(static_cast<NumberLiteral*>(expression), tokens);
            }
            case NodeKind::GroupingExpression:
            {
                return typeCheckGroupingExpression(static_cast<GroupingExpression*>(expression), tokens);
            }
            case NodeKind::UnaryExpression:
            {
                return typeCheckUnaryExpressionExpression(static_cast<UnaryExpression*>(expression), tokens);
            }
            case NodeKind::BinaryExpression:
            {
                return typeCheckBinaryExpressionExpression(static_cast<BinaryExpression*>(expression), tokens);
            }
            case NodeKind::NameExpression:
            {
                return typeCheckNameExpression(static_cast<NameExpression*>(expression), tokens);
            }
            case NodeKind::FunctionCallExpression:
            {
                return typeCheckFunctionCallExpression(static_cast<FunctionCallExpression*>(expression), tokens);
            }
            case NodeKind::MemberAccessExpression:
            {
                return typeCheckMemberAccessExpression(static_cast<MemberAccessExpression*>(expression), tokens);
            }
            case NodeKind::DiscardLiteral:
            {
                return Type::Discard();
            }
            default:
            {
                TODO("Missing Expression!!");
            }
        }
        return Type::Undefined();
    }

    Type TypeChecker::typeCheckGroupingExpression(GroupingExpression* groupingExpression, const TokenBuffer& tokens)
    {
        auto type = typeCheckExpression(groupingExpression->expression().get(), tokens);

        groupingExpression->setType(type);
        return type;
    }

    Type TypeChecker::typeCheckUnaryExpressionExpression(UnaryExpression* unaryExpression, const TokenBuffer& tokens)
    {
        switch (unaryExpression->unaryOperator())
        {
            case UnaryOperatorKind::LogicalNegation:
            {
                auto type = typeCheckExpression(unaryExpression->expression().get(), tokens);

                const auto operandType = type.toValue();
                if (operandType != Type::Undefined()
                    && m_module.tryGetOperatorSignature(operandType, UnaryOperatorKind::LogicalNegation) == nullptr)
                {
                    m_diagnostics.addUnaryOperandTypeMismatchError(
                        tokens.source(),
                        unaryExpression->sourceLocation(tokens),
                        "!",
                        FormatTypeName(m_module, operandType),
                        "a 'bool' operand");

                    unaryExpression->setType(Type::Undefined());
                    return Type::Undefined();
                }

                unaryExpression->setType(type);
                return type;
            }
            case UnaryOperatorKind::ValueNegation:
            {
                auto* operand = unaryExpression->expression().get();
                const auto previousContext = m_negatedLiteralContext;
                m_negatedLiteralContext = dynamic_cast<NumberLiteral*>(operand) != nullptr;
                m_negatedLiteralSignConsumed = false;

                auto type = typeCheckExpression(operand, tokens);

                m_negatedLiteralContext = previousContext;
                if (m_negatedLiteralSignConsumed)
                {
                    unaryExpression->setSignFolded(true);
                    m_negatedLiteralSignConsumed = false;
                }

                const auto operandType = type.toValue();
                if (operandType != Type::Undefined()
                    && m_module.tryGetOperatorSignature(operandType, UnaryOperatorKind::ValueNegation) == nullptr)
                {
                    m_diagnostics.addUnaryOperandTypeMismatchError(
                        tokens.source(),
                        unaryExpression->sourceLocation(tokens),
                        "-",
                        FormatTypeName(m_module, operandType),
                        "a numeric operand");

                    unaryExpression->setType(Type::Undefined());
                    return Type::Undefined();
                }

                unaryExpression->setType(type);
                return type;
            }
            case UnaryOperatorKind::ReferenceOf:
            {
                auto* operand = unaryExpression->expression().get();
                auto type = typeCheckExpression(operand, tokens);
                if (type.isReference())
                {
                    m_diagnostics.addAlreadyReferenceError(
                        tokens.source(),
                        unaryExpression->sourceLocation(tokens));
                    return Type::Undefined();
                }

                if (operand->kind() == NodeKind::NameExpression)
                {
                    const auto& name = static_cast<const NameExpression*>(operand)->name();
                    if (currentScope()->tryGetVariableBindingKind(name) == VariableBindingKind::LocalConstant)
                    {
                        unaryExpression->setReferencesConstant(true);
                    }
                }

                auto referenceType = type.toReference();
                unaryExpression->setType(referenceType);
                return referenceType;
            }
            default:
            {
                TODO("Missing UnaryOperatorKind!!");
                return Type::Undefined();
            }
        }
    }

    Type TypeChecker::typeCheckBinaryExpressionExpression(BinaryExpression* binaryExpression, const TokenBuffer& tokens)
    {
        switch (binaryExpression->binaryOperator())
        {
            case BinaryOperatorKind::MemberAccess:
            {
                auto leftType = typeCheckExpression(binaryExpression->leftExpression().get(), tokens);
                if (leftType.kind() == TypeKind::Enum)
                {
                    auto& enumDefinition = m_module.getEnumDefinition(leftType);
                    if (binaryExpression->rightExpression()->kind() == NodeKind::NameExpression)
                    {
                        auto* fieldNameExpression = static_cast<NameExpression*>(binaryExpression->rightExpression().get());
                        if (!enumDefinition.hasField(fieldNameExpression->name()))
                        {
                            m_diagnostics.addUnknownEnumFieldError(
                                tokens.source(),
                                fieldNameExpression->sourceLocation(tokens),
                                enumDefinition.name(),
                                fieldNameExpression->name());
                            return Type::Undefined();
                        }

                        binaryExpression->setType(leftType);
                        fieldNameExpression->setType(leftType);
                        return leftType;
                    }

                    m_diagnostics.addInvalidEnumMemberAccessError(
                        tokens.source(),
                        binaryExpression->rightExpression()->sourceLocation(tokens),
                        enumDefinition.name());
                    return Type::Undefined();
                }
                else if (leftType.kind() == TypeKind::Type)
                {
                    auto& typeDefinition = m_module.getTypeDefinition(leftType);

                    if (binaryExpression->rightExpression()->kind() == NodeKind::FunctionCallExpression)
                    {
                        auto functionCallExpression = static_cast<FunctionCallExpression*>(binaryExpression->rightExpression().get());
                        const auto& name = functionCallExpression->nameExpression()->name();

                        auto methodType = typeDefinition.tryGetMethodTypeByName(name);
                        if (methodType != Type::Undefined())
                        {
                            auto& methodDefinition = m_module.getFunctionDefinition(methodType);
                            if (!typeCheckCallArguments(functionCallExpression, methodDefinition, tokens))
                            {
                                return Type::Undefined();
                            }

                            const auto& returnTypes = methodDefinition.returnTypes();
                            Type returnType = Type::Void();
                            if (returnTypes.size() == 1)
                            {
                                returnType = returnTypes[0];
                            }
                            else if (returnTypes.size() > 1)
                            {
                                TODO("Handle multiple return types");
                            }

                            if (methodDefinition.functionType() == FunctionType::SynthesizedConstructor)
                            {
                                binaryExpression->setBinaryOperatorKind(BinaryOperatorKind::ConstructorCall);
                                binaryExpression->setType(leftType);
                                functionCallExpression->setType(leftType);
                                functionCallExpression->setFunctionType(methodType);
                                functionCallExpression->nameExpression()->setType(methodType);
                                return leftType;
                            }

                            binaryExpression->setType(returnType);
                            functionCallExpression->setType(returnType);
                            functionCallExpression->setFunctionType(methodType);
                            functionCallExpression->nameExpression()->setType(methodType);
                            return returnType;
                        }
                        else
                        {
                            m_diagnostics.addUnknownMethodError(
                                tokens.source(),
                                functionCallExpression->sourceLocation(tokens),
                                typeDefinition.name(),
                                name);
                            return Type::Undefined();
                        }
                    }
                    else if (binaryExpression->rightExpression()->kind() == NodeKind::NameExpression)
                    {
                        auto* fieldNameExpression = static_cast<NameExpression*>(binaryExpression->rightExpression().get());
                        const auto& fieldDefinition = typeDefinition.tryGetFieldByName(fieldNameExpression->name());
                        if (fieldDefinition.type() == Type::Undefined())
                        {
                            m_diagnostics.addUnknownFieldError(
                                tokens.source(),
                                fieldNameExpression->sourceLocation(tokens),
                                typeDefinition.name(),
                                fieldNameExpression->name());
                            return Type::Undefined();
                        }

                        auto fieldType = fieldDefinition.type();
                        fieldNameExpression->setType(fieldType);
                        binaryExpression->setType(fieldType);
                        return fieldType;
                    }
                }

                if (leftType != Type::Undefined())
                {
                    m_diagnostics.addInvalidMemberAccessReceiverError(
                        tokens.source(),
                        binaryExpression->sourceLocation(tokens),
                        FormatTypeName(m_module, leftType));
                }
                return Type::Undefined();
            }
            case BinaryOperatorKind::Addition:
            case BinaryOperatorKind::Subtraction:
            case BinaryOperatorKind::Multiplication:
            case BinaryOperatorKind::Division:
            {
                auto leftType = typeCheckExpression(binaryExpression->leftExpression().get(), tokens);
                if (leftType.isReference())
                {
                    leftType = leftType.toValue();
                }
                auto rightType = typeCheckExpression(binaryExpression->rightExpression().get(), tokens);
                if (rightType.isReference())
                {
                    rightType = rightType.toValue();
                }

                // TODO we need to be able look up the resulting type for a binary expression, 
                // for now we'll just make sure left and right have the same type and use that one
                if (leftType != rightType)
                {
                    if (leftType != Type::Undefined() && rightType != Type::Undefined())
                    {
                        m_diagnostics.addArithmeticOperandTypeMismatchError(
                            tokens.source(),
                            tokens.getSourceLocation(binaryExpression->binaryOperatorToken()),
                            FormatBinaryOperator(binaryExpression->binaryOperator()),
                            FormatTypeName(m_module, leftType),
                            FormatTypeName(m_module, rightType));
                    }

                    binaryExpression->setType(Type::Undefined());
                    return Type::Undefined();
                }

                const auto* operatorSignature = m_module.tryGetOperatorSignature(leftType, binaryExpression->binaryOperator());
                if (operatorSignature == nullptr)
                {
                    m_diagnostics.addBinaryOperandTypeMismatchError(
                        tokens.source(),
                        tokens.getSourceLocation(binaryExpression->binaryOperatorToken()),
                        FormatBinaryOperator(binaryExpression->binaryOperator()),
                        FormatTypeName(m_module, leftType),
                        "numeric operands");

                    binaryExpression->setType(Type::Undefined());
                    return Type::Undefined();
                }

                binaryExpression->setType(operatorSignature->resultType);
                return operatorSignature->resultType;
            }
            case BinaryOperatorKind::Equal:
            case BinaryOperatorKind::NotEqual:
            case BinaryOperatorKind::LessThan:
            case BinaryOperatorKind::LessOrEqual:
            case BinaryOperatorKind::GreaterThan:
            case BinaryOperatorKind::GreaterOrEqual:
            case BinaryOperatorKind::LogicalAnd:
            case BinaryOperatorKind::LogicalOr:
            {
                auto leftType = typeCheckExpression(binaryExpression->leftExpression().get(), tokens);
                if (leftType.isReference())
                {
                    leftType = leftType.toValue();
                }
                auto rightType = typeCheckExpression(binaryExpression->rightExpression().get(), tokens);
                if (rightType.isReference())
                {
                    rightType = rightType.toValue();
                }

                if (!areComparableTypes(leftType, rightType))
                {
                    if (leftType != Type::Undefined() && rightType != Type::Undefined())
                    {
                        m_diagnostics.addComparisonOperandTypeMismatchError(
                            tokens.source(),
                            tokens.getSourceLocation(binaryExpression->binaryOperatorToken()),
                            FormatBinaryOperator(binaryExpression->binaryOperator()),
                            FormatTypeName(m_module, leftType),
                            FormatTypeName(m_module, rightType));
                    }

                    binaryExpression->setType(Type::Undefined());
                    return Type::Undefined();
                }

                const auto operation = binaryExpression->binaryOperator();

                auto lookupType = leftType;
                if (lookupType.kind() == TypeKind::Enum)
                {
                    lookupType = m_module.getEnumDefinition(lookupType).baseType();
                }

                const auto* operatorSignature = m_module.tryGetOperatorSignature(lookupType, operation);
                if (operatorSignature == nullptr)
                {
                    const auto* expectedOperands = "numeric operands";
                    if (operation == BinaryOperatorKind::LogicalAnd || operation == BinaryOperatorKind::LogicalOr)
                    {
                        expectedOperands = "'bool' operands";
                    }
                    else if (operation == BinaryOperatorKind::Equal || operation == BinaryOperatorKind::NotEqual)
                    {
                        expectedOperands = "operands that can be compared for equality";
                    }

                    m_diagnostics.addBinaryOperandTypeMismatchError(
                        tokens.source(),
                        tokens.getSourceLocation(binaryExpression->binaryOperatorToken()),
                        FormatBinaryOperator(operation),
                        FormatTypeName(m_module, leftType),
                        expectedOperands);

                    binaryExpression->setType(Type::Undefined());
                    return Type::Undefined();
                }

                auto resultType = operatorSignature->resultType;
                binaryExpression->setType(resultType);
                return resultType;
            }
            default:
            {
                TODO("Missing BinaryOperatornKind!!");
                return Type::Undefined();
            }
        }

        return Type::Undefined();
    }

    Type TypeChecker::typeCheckNameExpression(NameExpression* expression, const TokenBuffer& tokens)
    {
        const auto& name = expression->name();
        auto optionalVariableType = currentScope()->tryGetVariableBinding(name);
        if (optionalVariableType.has_value())
        {
            static_cast<void>(currentScope()->markVariableBindingRead(name));
            auto type = optionalVariableType.value();
            expression->setType(type);
            return type;
        }

        auto type = m_module.tryGetTypeByName(name);
        if (type != Type::Undefined())
        {
            expression->setType(type);
            return type;
        }

        m_diagnostics.addUnknownNameError(tokens.source(), expression->sourceLocation(tokens), name);

        return Type::Undefined();
    }

    Type TypeChecker::typeCheckFunctionCallExpression(FunctionCallExpression* functionCallExpression, const TokenBuffer& tokens)
    {
        const auto& name = functionCallExpression->nameExpression()->name();
        auto functionType = m_module.tryGetFunctionTypeByName(name);
        if (functionType == Type::Undefined())
        {
            if (m_currentType != Type::Undefined())
            {
                auto& typeDefinition = m_module.getTypeDefinition(m_currentType);
                if (typeDefinition.tryGetMethodTypeByName(name) != Type::Undefined())
                {
                    m_diagnostics.addMethodCallMissingDotError(
                        tokens.source(),
                        functionCallExpression->sourceLocation(tokens),
                        name,
                        typeDefinition.name());
                    return Type::Undefined();
                }
            }

            m_diagnostics.addUnknownFunctionError(
                tokens.source(),
                functionCallExpression->sourceLocation(tokens),
                name);
            return Type::Undefined();
        }

        return typeCheckResolvedFunctionCall(functionCallExpression, functionType, tokens);
    }

    Type TypeChecker::typeCheckResolvedFunctionCall(FunctionCallExpression* functionCallExpression, Type functionType, const TokenBuffer& tokens)
    {
        const auto& functionDefinition = m_module.getFunctionDefinition(functionType);

        if (!typeCheckCallArguments(functionCallExpression, functionDefinition, tokens))
        {
            return Type::Undefined();
        }

        const auto& returnTypes = functionDefinition.returnTypes();
        Type returnType = Type::Void();
        if (returnTypes.size() == 1)
        {
            returnType = returnTypes[0];
        }
        else if (returnTypes.size() > 1)
        {
            TODO("Handle multiple return types");
        }

        functionCallExpression->setType(returnType);
        functionCallExpression->setFunctionType(functionType);
        functionCallExpression->nameExpression()->setType(functionType);
        return returnType;
    }

    Type TypeChecker::typeCheckMemberAccessExpression(MemberAccessExpression* memberAccessExpression, const TokenBuffer& tokens)
    {
        if (m_currentType == Type::Undefined())
        {
            TODO("Member access expression only supported in type scope");
            return Type::Undefined();
        }

        auto& typeDefinition = m_module.getTypeDefinition(m_currentType);
        if (memberAccessExpression->expression()->kind() == NodeKind::FunctionCallExpression)
        {
            auto* functionCallExpression = static_cast<FunctionCallExpression*>(memberAccessExpression->expression().get());
            const auto& methodName = functionCallExpression->nameExpression()->name();
            const auto methodType = typeDefinition.tryGetMethodTypeByName(methodName);
            if (methodType == Type::Undefined())
            {
                m_diagnostics.addUnknownMethodError(
                    tokens.source(),
                    functionCallExpression->sourceLocation(tokens),
                    typeDefinition.name(),
                    methodName);

                memberAccessExpression->setType(Type::Undefined());
                return Type::Undefined();
            }

            const auto type = typeCheckResolvedFunctionCall(functionCallExpression, methodType, tokens);
            memberAccessExpression->setType(type);
            return type;
        }

        auto type = typeCheckExpression(memberAccessExpression->expression().get(), tokens);
        memberAccessExpression->setType(type);

        return type;
    }

    bool TypeChecker::typeCheckCallArguments(
        FunctionCallExpression* functionCallExpression,
        const FunctionDefinition& functionDefinition,
        const TokenBuffer& tokens)
    {
        const auto& parameterTypes = functionDefinition.parameters();
        auto isVariadic = functionDefinition.isVariadic();
        auto parameterCount = (isVariadic ? parameterTypes.size() - 1 : parameterTypes.size());
        const bool hasImplicitThis =
            functionDefinition.functionType() == FunctionType::SynthesizedConstructor ||
            functionDefinition.functionType() == FunctionType::PublicMethod ||
            functionDefinition.functionType() == FunctionType::PrivateMethod;
        const size_t parameterOffset = hasImplicitThis ? 1 : 0;
        if (parameterCount < parameterOffset)
        {
            TODO("This shouldn't happen");
            return false;
        }

        auto binding = bindCallArguments(parameterTypes, parameterOffset, isVariadic, *functionCallExpression, functionDefinition.name(), m_diagnostics, tokens);
        if (!binding.ok)
        {
            return false;
        }

        for (const auto& argument : functionCallExpression->arguments())
        {
            typeCheckExpression(argument.value().get(), tokens);
        }

        const auto expectedArgumentCount = parameterCount - parameterOffset;
        std::vector<ArgumentTypeMismatchInfo> argumentTypeMismatches{};
        for (size_t i = 0; i < binding.ordered.size(); ++i)
        {
            const auto* argument = binding.ordered[i];
            const auto argumentType = argument->type();
            const auto expectedType = parameterTypes[i + parameterOffset].type();
            if (argumentType != expectedType)
            {
                argumentTypeMismatches.push_back(ArgumentTypeMismatchInfo{
                    argument->sourceLocation(tokens),
                    static_cast<i32>(i + 1),
                    FormatTypeName(m_module, expectedType),
                    FormatTypeName(m_module, argumentType),
                    });
            }
        }

        for (size_t j = 0; j < binding.variadic.size(); ++j)
        {
            const auto* argument = binding.variadic[j];
            const auto argumentType = argument->type();
            if (argumentType == Type::Undefined() || argumentType == Type::Void())
            {
                m_diagnostics.addInvalidVariadicArgumentTypeError(
                    tokens.source(),
                    argument->sourceLocation(tokens),
                    functionDefinition.name(),
                    static_cast<i32>(expectedArgumentCount + j + 1),
                    FormatTypeName(m_module, argumentType));
                return false;
            }
        }

        if (!argumentTypeMismatches.empty())
        {
            const auto argumentLocation = functionCallExpression->argumentsLocation(tokens);
            m_diagnostics.addArgumentTypeMismatchError(
                tokens.source(),
                argumentLocation,
                functionDefinition.name(),
                argumentTypeMismatches);
            return false;
        }

        functionCallExpression->setBoundArguments(std::move(binding.ordered), std::move(binding.variadic));
        return true;
    }

    Type TypeChecker::typeCheckNumberLiteral(NumberLiteral* literal, const TokenBuffer& tokens)
    {
        auto numberType = Type::Undefined();
        auto parsedValue = std::optional<NumberLiteral::ParsedValue>{};
        if (literal->explicitType().has_value())
        {
            numberType = typeCheckTypeNameNode(literal->explicitType().value().get(), tokens);
        }
        else
        {
            const auto& lexeme = literal->literalLexeme();
            const auto isFloatingLiteral = lexeme.find('.') != std::string_view::npos;
            if (m_contextualNumberType.has_value())
            {
                const auto contextualType = m_contextualNumberType.value();
                if (!isFloatingLiteral && (contextualType == Type::U8() || contextualType == Type::I32()))
                {
                    numberType = contextualType;
                }
            }

            if (numberType == Type::Undefined() && isFloatingLiteral)
            {
                numberType = m_options.defaultFloatingType;
            }
            else if (numberType == Type::Undefined())
            {
                numberType = m_options.defaultIntegerType;
            }
        }

        if (numberType == Type::U8() || numberType == Type::I32() || numberType == Type::F32())
        {
            parsedValue = TryParseNumberLiteralValue(literal->literalLexeme(), numberType);
            if (!parsedValue.has_value() && m_negatedLiteralContext && numberType == Type::I32())
            {
                const auto negatedValue = TryParseNegatedI32Literal(literal->literalLexeme());
                if (negatedValue.has_value())
                {
                    parsedValue = NumberLiteral::ParsedValue{ negatedValue.value() };
                    m_negatedLiteralSignConsumed = true;
                }
            }

            if (!parsedValue.has_value())
            {
                m_diagnostics.addNumberLiteralOutOfRangeError(
                    tokens.source(),
                    literal->sourceLocation(tokens),
                    literal->literalLexeme(),
                    FormatTypeName(m_module, numberType));
                numberType = Type::Undefined();
            }
        }

        literal->setParsedValue(std::move(parsedValue));
        literal->setType(numberType);
        return numberType;
    }

    Type TypeChecker::typeCheckTypeNameNode(TypeNameNode* typeNameNode, const TokenBuffer& tokens)
    {
        const auto& name = typeNameNode->name();
        auto type = m_module.tryGetTypeByName(name);
        if (type != Type::Undefined())
        {
            if (typeNameNode->isReference())
            {
                if (type.isReference())
                {
                    TODO("Add error diagnostics for already being a reference");
                    return Type::Undefined();
                }
                type = type.toReference();
            }
            // TODO nullable handling
            typeNameNode->setType(type);
            return type;
        }
        else
        {
            m_diagnostics.addUnknownTypeError(tokens.source(), tokens.getSourceLocation(typeNameNode->nameToken()), name);
            return Type::Undefined();
        }
    }

    std::vector<Parameter> TypeChecker::typeCheckParametersNode(ParametersNode* parametersNode, const TokenBuffer& tokens)
    {
        std::vector<Parameter> parameters{};
        auto seenDefault = false;

        // first loop doesnt put the parameter names into the scope, so that default values cannot reference other parameters
        for (const auto& parameterNode : parametersNode->parameters())
        {
            auto parameterType = typeCheckTypeNameNode(parameterNode->typeName().get(), tokens);
            if (parameterType == Type::Void())
            {
                m_diagnostics.addVoidParameterTypeError(
                    tokens.source(),
                    parameterNode->sourceLocation(tokens),
                    parameterNode->name());

                parameterType = Type::Undefined();
            }
            parameterNode->setType(parameterType);

            const Expression* defaultValue = nullptr;
            if (parameterNode->hasDefault())
            {
                auto* defaultExpression = parameterNode->defaultValue().get();
                m_contextualNumberType = parameterType;
                const auto defaultType = typeCheckExpression(defaultExpression, tokens);
                m_contextualNumberType = std::nullopt;
                if (defaultType != Type::Undefined() && defaultType != parameterType)
                {
                    m_diagnostics.addDefaultParameterTypeMismatchError(
                        tokens.source(),
                        defaultExpression->sourceLocation(tokens),
                        FormatTypeName(m_module, parameterType),
                        FormatTypeName(m_module, defaultType));
                }
                defaultValue = defaultExpression;
                seenDefault = true;
            }
            else if (seenDefault && !parameterNode->isVariadic())
            {
                // once a parameter with a default value is seen, all following parameters must have defaults
                m_diagnostics.addNonTrailingDefaultParameterError(
                    tokens.source(),
                    parameterNode->sourceLocation(tokens),
                    parameterNode->name());
            }

            parameters.push_back(Parameter{ parameterNode->name(), parameterType, defaultValue });
        }

        // finally add parameter names into the current scope
        for (const auto& parameterNode : parametersNode->parameters())
        {
            const auto& name = parameterNode->name();
            auto scope = currentScope();
            if (!scope->hasVariableBinding(name))
            {
                scope->addVariableBinding(name, parameterNode->type(), tokens.getSourceLocation(parameterNode->nameToken()), tokens.source(), VariableBindingKind::Parameter);
            }
            else
            {
                m_diagnostics.addDuplicateParameterDeclarationError(
                    tokens.source(),
                    tokens.getSourceLocation(parameterNode->nameToken()),
                    name,
                    scope->tryGetVariableBindingSource(name),
                    scope->tryGetVariableBindingLocation(name));
            }
        }

        return parameters;
    }

    std::vector<Type> TypeChecker::typeCheckReturnTypesNode(ReturnTypesNode* returnTypesNode, const TokenBuffer& tokens)
    {
        std::vector<Type> types{};
        for (const auto& returnTypeNode : returnTypesNode->returnTypes())
        {
            auto returnType = typeCheckTypeNameNode(returnTypeNode.get(), tokens);
            if (returnType.isReference())
            {
                m_diagnostics.addReferenceReturnTypeError(
                    tokens.source(),
                    returnTypeNode->sourceLocation(tokens),
                    returnTypeNode->name());
            }
            types.push_back(returnType);
        }
        return types;
    }

    i32 TypeChecker::convertToI32(NumberLiteral* literal, const TokenBuffer& tokens)
    {
        auto literalType = typeCheckNumberLiteral(literal, tokens);
        if (literalType != Type::I32())
        {
            return 0;
        }

        if (!literal->hasParsedValue())
        {
            m_diagnostics.addNumberLiteralOutOfRangeError(
                tokens.source(),
                literal->sourceLocation(tokens),
                literal->literalLexeme(),
                FormatTypeName(m_module, Type::I32()));
            return 0;
        }

        return std::get<i32>(literal->parsedValue().value());
    }

    Type TypeChecker::coerceConditionType(Type conditionType, Expression* conditionExpression)
    {
        if (conditionType == Type::Bool())
        {
            return conditionType;
        }

        if (conditionType.kind() == TypeKind::Enum)
        {
            auto& enumDefinition = m_module.getEnumDefinition(conditionType);
            if (enumDefinition.baseType() == Type::Bool())
            {
                conditionExpression->setType(Type::Bool());
                return Type::Bool();
            }
        }

        return conditionType;
    }

    bool TypeChecker::areComparableTypes(Type leftType, Type rightType)
    {
        if (leftType == rightType)
        {
            return true;
        }

        if (leftType.kind() == TypeKind::Enum)
        {
            auto& enumDefinition = m_module.getEnumDefinition(leftType);
            if (enumDefinition.baseType() == rightType)
            {
                return true;
            }
        }

        if (rightType.kind() == TypeKind::Enum)
        {
            auto& enumDefinition = m_module.getEnumDefinition(rightType);
            if (enumDefinition.baseType() == leftType)
            {
                return true;
            }
        }

        return false;
    }

    const TokenBuffer* TypeChecker::tryTokensFor(const Statement* statement) const
    {
        const auto it = m_statementTokens.find(statement);
        if (it == m_statementTokens.end())
        {
            return nullptr;
        }

        return it->second;
    }

    const TokenBuffer& TypeChecker::tokensFor(const Statement* statement) const
    {
        const auto it = m_statementTokens.find(statement);
        if (it == m_statementTokens.end())
        {
            TODO("Missing token buffer for statement");
        }

        return *it->second;
    }

    static bool IsTerminatingStatement(const Statement* statement)
    {
        switch (statement->kind())
        {
            case NodeKind::ReturnStatement:
            case NodeKind::BreakStatement:
            case NodeKind::SkipStatement:
                return true;
            default:
                return false;
        }
    }

    static std::optional<SourceLocation> DeadStatementLocation(const Statement* statement, const TokenBuffer& tokens)
    {
        switch (statement->kind())
        {
            case NodeKind::ReturnStatement:
                return tokens.getSourceLocation(static_cast<const ReturnStatement*>(statement)->keywordToken());
            case NodeKind::BreakStatement:
                return tokens.getSourceLocation(static_cast<const BreakStatement*>(statement)->keywordToken());
            case NodeKind::SkipStatement:
                return tokens.getSourceLocation(static_cast<const SkipStatement*>(statement)->keywordToken());
            case NodeKind::IfStatement:
                return tokens.getSourceLocation(static_cast<const IfStatement*>(statement)->ifKeyword());
            case NodeKind::WhileStatement:
                return tokens.getSourceLocation(static_cast<const WhileStatement*>(statement)->whileKeyword());
            case NodeKind::ExpressionStatement:
                return static_cast<const ExpressionStatement*>(statement)->expression()->sourceLocation(tokens);
            case NodeKind::AssignmentStatement:
                return static_cast<const AssignmentStatement*>(statement)->leftExpression()->sourceLocation(tokens);
            case NodeKind::ConstantDeclaration:
                return static_cast<const ConstantDeclaration*>(statement)->leftExpression()->sourceLocation(tokens);
            case NodeKind::VariableDeclaration:
                return static_cast<const VariableDeclaration*>(statement)->leftExpression()->sourceLocation(tokens);
            default:
                return std::nullopt;
        }
    }

    void TypeChecker::typeCheckBlockNode(BlockNode* body, const TokenBuffer& tokens)
    {
        auto reachedTerminator = false;
        auto warnedUnreachable = false;
        for (const auto& statement : body->statements())
        {
            if (reachedTerminator && !warnedUnreachable)
            {
                if (const auto location = DeadStatementLocation(statement.get(), tokens); location.has_value())
                {
                    m_diagnostics.addUnreachableCodeWarning(tokens.source(), location.value());
                    warnedUnreachable = true;
                }
            }

            typeCheckStatement(statement.get(), tokens);

            if (IsTerminatingStatement(statement.get()))
            {
                reachedTerminator = true;
            }
        }
    }

    void TypeChecker::pushScope(ScopeKind kind)
    {
        auto parent = m_scopes.back().get();
        m_scopes.emplace_back(std::make_unique<Scope>(parent, kind));
    }

    void TypeChecker::emitUnusedVariableWarnings(const Scope& scope)
    {
        if (m_diagnostics.hasErrors())
        {
            return;
        }

        for (const auto& [name, binding] : scope.variableBindings())
        {
            if (binding.wasRead || ShouldIgnoreUnusedVariableWarning(name) || !binding.location.has_value())
            {
                continue;
            }

            if (binding.kind != VariableBindingKind::LocalVariable && binding.kind != VariableBindingKind::LocalConstant && binding.kind != VariableBindingKind::Parameter)
            {
                continue;
            }

            if (binding.kind == VariableBindingKind::Parameter)
            {
                m_diagnostics.addUnusedParameterWarning(binding.source, binding.location.value(), std::string(name));
            }
            else if (binding.kind == VariableBindingKind::LocalConstant)
            {
                m_diagnostics.addUnusedLocalConstantWarning(binding.source, binding.location.value(), std::string(name));
            }
            else
            {
                m_diagnostics.addUnusedLocalVariableWarning(binding.source, binding.location.value(), std::string(name));
            }
        }
    }

    void TypeChecker::popScope(bool emitUnusedWarnings)
    {
        if (emitUnusedWarnings)
        {
            emitUnusedVariableWarnings(*m_scopes.back());
        }

        m_scopes.pop_back();
        if (m_scopes.size() == 0)
        {
            TODO("Popped too many scopes");
        }
    }

    Scope* TypeChecker::currentScope() const noexcept
    {
        return m_scopes.back().get();
    }
}
