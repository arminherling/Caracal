#include "TypeChecker.h"

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
    };

    static const AnnotationDefinition* GetAnnotationDefinition(AnnotationKind kind)
    {
        static const AnnotationDefinition Definitions[] = {
            { AnnotationKind::Extern, "extern", TokenKind::DefKeyword, 0, Type::Undefined() },
            { AnnotationKind::Flag, "flag", TokenKind::EnumKeyword, 0, Type::Undefined() },
            { AnnotationKind::Step, "step", TokenKind::EnumKeyword, 1, Type::I32() },
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

    static std::string FormatTypeName(Module& module, Type type)
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

    bool typeCheck(
        const std::vector<ParseTreeUPtr>& parseTrees,
        const TypeCheckerOptions& options,
        Module& module,
        DiagnosticsBag& diagnostics) noexcept
    {
        TypeChecker typeChecker{ parseTrees, options, module, diagnostics };
        return typeChecker.typeCheck();
    }

    TypeChecker::TypeChecker(
        const std::vector<ParseTreeUPtr>& parseTrees,
        const TypeCheckerOptions& options,
        Module& module,
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
        if (!m_diagnostics.Diagnostics().empty())
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
        
        return m_diagnostics.Diagnostics().empty();
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
                                m_diagnostics.AddUnexpectedAnnotationTargetError(
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

                            m_diagnostics.AddDuplicateTypeDeclarationError(
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

                            m_diagnostics.AddDuplicateTypeDeclarationError(
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

                            m_diagnostics.AddDuplicateFunctionDeclarationError(
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

                    m_diagnostics.AddExplicitConstructorDeclarationError(
                        tokens.source(),
                        tokens.getSourceLocation(methodStatement->methodNameNode()->methodNameToken()),
                        constructorLocation,
                        std::string(typeDefinitionStatement->name()));
                    continue;
                }

                std::vector<Parameter> declarationParameters{};
                if (modifier != MethodModifier::Static)
                {
                    declarationParameters.emplace_back("this", typeDefinition.type().toReference());
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

            std::vector<Parameter> constructorParameters{};
            constructorParameters.emplace_back("this", typeType.toReference());

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
        popScope();

        const auto& functionName = statement->name();
        auto functionType = m_module.tryGetFunctionTypeByName(functionName);
        if (functionType == Type::Undefined())
        {
            TODO("This shouldn't happen");
        }

        auto& functionDefinition = m_module.getFunctionDefinition(functionType);
        const auto& parameterNodes = statement->parametersNode()->parameters();
        const auto isExtern = validateFunctionAnnotation(statement, tokens);
        const auto isVariadic = !parameterNodes.empty() && parameterNodes.back()->isVariadic();
        if (isVariadic && !isExtern)
        {
            auto* variadicParameter = parameterNodes.back().get();
            m_diagnostics.AddNonExternVariadicFunctionError(
                tokens.source(),
                variadicParameter->sourceLocation(tokens),
                functionName);
        }
        functionDefinition.setParameters(parameters);
        functionDefinition.setReturnTypes(returns);
        functionDefinition.setIsVariadic(isVariadic);
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
        typeCheckConstructorSignature(statement, typeDefinition, typeType, tokens);

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
        }
    }

    void TypeChecker::typeCheckTypeFieldDefinition(TypeDefinitionStatement* statement, const TokenBuffer& tokens)
    {
        auto typeType = statement->type();
        if (typeType == Type::Undefined())
        {
            TODO("This shouldn't happen");
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

        popScope();
    }

    void TypeChecker::typeCheckTypeMethodDefinition(TypeDefinitionStatement* statement, const TokenBuffer& tokens)
    {
        auto typeType = statement->type();
        if (typeType == Type::Undefined())
        {
            TODO("This shouldn't happen");
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
                tokens.source());
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

        popScope();
        m_currentType = Type::Undefined();
    }

    void TypeChecker::typeCheckMethodSignature(const MethodDefinitionStatement* methodStatement, TypeDefinition& typeDefinition, Type typeType, const TokenBuffer& tokens)
    {
        const auto& methodName = methodStatement->methodNameNode()->methodName();

        pushScope(ScopeKind::Method);
        auto parameters = typeCheckParametersNode(methodStatement->parametersNode().get(), tokens);
        auto returns = typeCheckReturnTypesNode(methodStatement->returnTypesNode().get(), tokens);
        popScope();

        auto methodType = typeDefinition.tryGetMethodTypeByName(methodName);
        if (methodType == Type::Undefined())
        {
            TODO("This shouldn't happen");
        }

        auto& methodDefinition = m_module.getFunctionDefinition(methodType);
        if (methodStatement->modifier() != MethodModifier::Static)
        {
            parameters.insert(parameters.begin(), Parameter{ "this", typeType.toReference() });
        }
        methodDefinition.setParameters(parameters);
        methodDefinition.setReturnTypes(returns);
    }

    void TypeChecker::typeCheckConstructorSignature(const TypeDefinitionStatement* typeDefinitionStatement, TypeDefinition& typeDefinition, Type typeType, const TokenBuffer& tokens)
    {
        auto constructorType = typeDefinition.tryGetMethodTypeByName("new");
        if (constructorType == Type::Undefined())
        {
            TODO("This shouldn't happen");
        }

        std::vector<Parameter> constructorParameters{};
        constructorParameters.emplace_back("this", typeType.toReference());

        if (typeDefinitionStatement->constructorParameters().has_value())
        {
            pushScope(ScopeKind::Type);
            auto declaredConstructorParameters = typeCheckParametersNode(typeDefinitionStatement->constructorParameters().value().get(), tokens);
            popScope();

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
                m_diagnostics.AddUnexpectedAnnotationTargetError(
                    tokens.source(),
                    annotation->sourceLocation(tokens));
            }
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
                scope->addVariableBinding(name, rightType, nameExpression->sourceLocation(tokens), tokens.source());

                if (statement->isGlobalConstant())
                {
                    // TODO maybe remove the return value from the function?
                    static_cast<void>(m_module.createConstant(name, rightExpression));
                }
            }
            else
            {
                m_diagnostics.AddDuplicateConstantDeclarationError(
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
                m_diagnostics.AddExplicitConstantTypeMismatchError(
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
        auto rightType = typeCheckExpression(statement->rightExpression().get(), tokens);

        auto leftExpression = statement->leftExpression().get();
        if (leftExpression->kind() == NodeKind::NameExpression)
        {
            auto nameExpression = static_cast<NameExpression*>(leftExpression);
            const auto& name = nameExpression->name();
            auto scope = currentScope();
            if (!scope->hasVariableBinding(name))
            {
                scope->addVariableBinding(name, rightType, nameExpression->sourceLocation(tokens), tokens.source());
            }
            else
            {
                m_diagnostics.AddDuplicateVariableDeclarationError(
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
                m_diagnostics.AddExplicitVariableTypeMismatchError(
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
        auto leftType = typeCheckExpression(statement->leftExpression().get(), tokens);
        if (leftType.isReference())
        {
            leftType = leftType.toValue();
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
            m_diagnostics.AddAssignmentTypeMismatchError(
                tokens.source(),
                statement->rightExpression()->sourceLocation(tokens),
                FormatTypeName(m_module, leftType),
                FormatTypeName(m_module, rightType));
        }
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
            currentScope()->addVariableBinding(parameters[i].name(), parameters[i].type(), location, tokens.source());
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

        typeCheckBlockNode(statement->bodyNode().get(), tokens);

        popScope();
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
                m_diagnostics.AddDuplicateEnumFieldDeclarationError(
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
                    m_diagnostics.AddFlagEnumExplicitValueError(
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
                        m_diagnostics.AddEnumFieldValueTypeMismatchError(
                            tokens.source(),
                            expression->sourceLocation(tokens),
                            FormatTypeName(m_module, baseType),
                            FormatTypeName(m_module, fieldValueType));
                    }
                }

                if (expression->kind() == NodeKind::NumberLiteral && expression->type() == Type::I32())
                {
                    auto value = convertToI32(static_cast<NumberLiteral*>(expression), tokens);
                    enumDefinition.addField(fieldName, value, fieldLocation);
                    fieldNode->setValue(value);
                    currentFieldValue = value + step;
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

    bool TypeChecker::validateAnnotation(const AnnotationNode* annotation, TokenKind targetKind, const TokenBuffer& tokens, std::optional<i32>* i32ArgumentValue)
    {
        const auto annotationLocation = annotation->sourceLocation(tokens);
        const auto* definition = GetAnnotationDefinition(annotation->kind());
        if (definition == nullptr)
        {
            m_diagnostics.AddUnknownAnnotationError(tokens.source(), annotationLocation, annotation->name(), targetKind);
            return false;
        }

        if (definition->targetKind != targetKind)
        {
            m_diagnostics.AddUnexpectedAnnotationTargetError(tokens.source(), annotationLocation);
            return false;
        }

        i32 actualCount = 0;
        if (annotation->argumentsNode().has_value())
        {
            actualCount = static_cast<i32>(annotation->argumentsNode().value()->arguments().size());
        }

        if (definition->requiredArgumentCount > 0 && !annotation->argumentsNode().has_value())
        {
            m_diagnostics.AddAnnotationMissingArgumentsError(tokens.source(), annotationLocation, annotation->kind(), annotation->name());
            return false;
        }

        if (actualCount != definition->requiredArgumentCount)
        {
            m_diagnostics.AddAnnotationWrongNumberOfArgumentsError(
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
            auto* argument = annotation->argumentsNode().value()->arguments().at(0).get();
            auto argumentType = typeCheckExpression(argument, tokens);
            if (argumentType == Type::Undefined())
            {
                return false;
            }

            if (argument->kind() != NodeKind::NumberLiteral)
            {
                m_diagnostics.AddAnnotationArgumentTypeMismatchError(
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
                m_diagnostics.AddAnnotationArgumentTypeMismatchError(
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

    bool TypeChecker::validateFunctionAnnotation(const FunctionDefinitionStatement* statement, const TokenBuffer& tokens)
    {
        auto isExtern = false;
        for (const auto& annotationNode : statement->annotations())
        {
            const auto* annotation = annotationNode.get();
            if (!validateAnnotation(annotation, TokenKind::DefKeyword, tokens))
            {
                continue;
            }

            if (annotation->kind() == AnnotationKind::Extern)
            {
                isExtern = true;
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
                        m_diagnostics.AddConflictingEnumAnnotationsError(
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
                        m_diagnostics.AddConflictingEnumAnnotationsError(
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
            m_diagnostics.AddDuplicateTypeFieldDeclarationError(
                tokens.source(),
                statement->nameExpression()->sourceLocation(tokens),
                fieldName,
                GetTypeFieldLocation(typeDefinition, existingField, tokens));
            return;
        }

        auto fieldType = Type::Undefined();
        Expression* fieldExpression = nullptr;
        if (statement->explicitType().has_value())
        {
            fieldType = typeCheckTypeNameNode(statement->explicitType().value().get(), tokens);
        }

        if (statement->rightExpression().has_value())
        {
            fieldExpression = statement->rightExpression().value().get();
            auto expressionType = typeCheckExpression(fieldExpression, tokens);
            if (fieldType == Type::Undefined())
            {
                fieldType = expressionType;
            }
            else if (fieldType != expressionType)
            {
                m_diagnostics.AddTypeFieldInitializerMismatchError(
                    tokens.source(),
                    fieldExpression->sourceLocation(tokens),
                    FormatTypeName(m_module, fieldType),
                    FormatTypeName(m_module, expressionType));
            }
        }

        if (fieldType == Type::Undefined())
        {
            return;
        }

        statement->nameExpression()->setType(fieldType);
        statement->setType(fieldType);
        typeDefinition.addField(fieldType, fieldName, fieldIndex, fieldExpression);
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
            currentScope()->addVariableBinding(parameters[i].name(), parameters[i].type(), location, tokens.source());
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

        typeCheckBlockNode(statement->bodyNode().get(), tokens);

        popScope();
        m_currentReturnType = Type::Void();
    }

    void TypeChecker::typeCheckIfStatement(IfStatement* statement, const TokenBuffer& tokens)
    {
        auto conditionType = typeCheckExpression(statement->condition().get(), tokens);
        conditionType = coerceConditionType(conditionType, statement->condition().get());
        if (conditionType != Type::Undefined() && conditionType != Type::Bool())
        {
            m_diagnostics.AddNonBoolIfConditionError(
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
            m_diagnostics.AddNonBoolWhileConditionError(
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
            if(type.isReference())
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
                m_diagnostics.AddReturnTypeMismatchError(
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
                m_diagnostics.AddReturnTypeMismatchError(
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
            case UnaryOperatorKind::ValueNegation:
            {
                auto type = typeCheckExpression(unaryExpression->expression().get(), tokens);

                unaryExpression->setType(type);
                return type;
            }
            case UnaryOperatorKind::ReferenceOf:
            {
                auto type = typeCheckExpression(unaryExpression->expression().get(), tokens);
                if (type.isReference())
                {
                    m_diagnostics.AddAlreadyReferenceError(
                        tokens.source(),
                        unaryExpression->sourceLocation(tokens));
                    return Type::Undefined();
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
                            m_diagnostics.AddUnknownEnumFieldError(
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

                    m_diagnostics.AddInvalidEnumMemberAccessError(
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

                            if (methodDefinition.functionType() == FunctionType::Constructor)
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
                            m_diagnostics.AddUnknownMethodError(
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
                            m_diagnostics.AddUnknownFieldError(
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
                    m_diagnostics.AddInvalidMemberAccessReceiverError(
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
                        m_diagnostics.AddArithmeticOperandTypeMismatchError(
                            tokens.source(),
                            tokens.getSourceLocation(binaryExpression->binaryOperatorToken()),
                            FormatBinaryOperator(binaryExpression->binaryOperator()),
                            FormatTypeName(m_module, leftType),
                            FormatTypeName(m_module, rightType));
                    }

                    binaryExpression->setType(Type::Undefined());
                    return Type::Undefined();
                }

                binaryExpression->setType(leftType);
                return leftType;
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
                        m_diagnostics.AddComparisonOperandTypeMismatchError(
                            tokens.source(),
                            tokens.getSourceLocation(binaryExpression->binaryOperatorToken()),
                            FormatBinaryOperator(binaryExpression->binaryOperator()),
                            FormatTypeName(m_module, leftType),
                            FormatTypeName(m_module, rightType));
                    }

                    binaryExpression->setType(Type::Undefined());
                    return Type::Undefined();
                }

                auto resultType = Type::Bool();
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

        m_diagnostics.AddUnknownNameError(tokens.source(), expression->sourceLocation(tokens), name);

        return Type::Undefined();
    }

    Type TypeChecker::typeCheckFunctionCallExpression(FunctionCallExpression* functionCallExpression, const TokenBuffer& tokens)
    {
        const auto& name = functionCallExpression->nameExpression()->name();
        auto functionType = m_module.tryGetFunctionTypeByName(name);
        if (functionType == Type::Undefined())
        {
            m_diagnostics.AddUnknownFunctionError(
                tokens.source(),
                functionCallExpression->sourceLocation(tokens),
                name);
            return Type::Undefined();
        }
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
            functionDefinition.functionType() == FunctionType::Constructor ||
            functionDefinition.functionType() == FunctionType::PublicMethod ||
            functionDefinition.functionType() == FunctionType::PrivateMethod;
        const size_t parameterOffset = hasImplicitThis ? 1 : 0;
        if (parameterCount < parameterOffset)
        {
            TODO("This shouldn't happen");
            return false;
        }

        auto argumentsNode = functionCallExpression->argumentsNode().get();
        const auto& arguments = argumentsNode->arguments();
        const auto expectedArgumentCount = parameterCount - parameterOffset;
        const auto argumentsLocation = argumentsNode->sourceLocation(tokens);

        if (isVariadic)
        {
            if (arguments.size() < expectedArgumentCount)
            {
                m_diagnostics.AddArgumentCountMismatchError(
                    tokens.source(),
                    argumentsLocation,
                    functionDefinition.name(),
                    static_cast<i32>(expectedArgumentCount),
                    static_cast<i32>(arguments.size()),
                    true);
                return false;
            }
        }
        else
        {
            if (arguments.size() != expectedArgumentCount)
            {
                m_diagnostics.AddArgumentCountMismatchError(
                    tokens.source(),
                    argumentsLocation,
                    functionDefinition.name(),
                    static_cast<i32>(expectedArgumentCount),
                    static_cast<i32>(arguments.size()),
                    false);
                return false;
            }
        }

        std::vector<ArgumentTypeMismatchInfo> argumentTypeMismatches{};
        for (size_t i = 0; i < arguments.size(); ++i)
        {
            auto* argument = arguments[i].get();
            auto argumentType = typeCheckExpression(argument, tokens);

            if (i < expectedArgumentCount)
            {
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
            else
            {
                if (argumentType == Type::Undefined() || argumentType == Type::Void())
                {
                    m_diagnostics.AddInvalidVariadicArgumentTypeError(
                        tokens.source(),
                        argument->sourceLocation(tokens),
                        functionDefinition.name(),
                        static_cast<i32>(i + 1),
                        FormatTypeName(m_module, argumentType));
                    return false;
                }
            }
        }

        if (!argumentTypeMismatches.empty())
        {
            m_diagnostics.AddArgumentTypeMismatchError(
                tokens.source(),
                argumentsLocation,
                functionDefinition.name(),
                argumentTypeMismatches);
            return false;
        }

        return true;
    }

    Type TypeChecker::typeCheckNumberLiteral(NumberLiteral* literal, const TokenBuffer& tokens)
    {
        auto numberType = Type::Undefined();
        if (literal->explicitType().has_value())
        {
            numberType = typeCheckTypeNameNode(literal->explicitType().value().get(), tokens);
        }
        else
        {
            const auto& lexeme = literal->literalLexeme();
            if (lexeme.find('.') != std::string_view::npos)
            {
                numberType = m_options.defaultFloatingType;
            }
            else
            {
                numberType = m_options.defaultIntegerType;
            }
        }

        if ((numberType == Type::U8() || numberType == Type::I32() || numberType == Type::F32())
            && !DoesLiteralFitType(literal->literalLexeme(), numberType))
        {
            m_diagnostics.AddNumberLiteralOutOfRangeError(
                tokens.source(),
                literal->sourceLocation(tokens),
                literal->literalLexeme(),
                FormatTypeName(m_module, numberType));
            numberType = Type::Undefined();
        }

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
            m_diagnostics.AddUnknownTypeError(tokens.source(), tokens.getSourceLocation(typeNameNode->nameToken()), name);
            return Type::Undefined();
        }
    }

    std::vector<Parameter> TypeChecker::typeCheckParametersNode(ParametersNode* parametersNode, const TokenBuffer& tokens)
    {
        std::vector<Parameter> parameters{};
        for (const auto& parameterNode : parametersNode->parameters())
        {
            auto parameterType = typeCheckTypeNameNode(parameterNode->typeName().get(), tokens);
            parameters.push_back(Parameter{ parameterNode->name(), parameterType });

            // register parameter in current scope
            const auto& name = parameterNode->name();
            auto scope = currentScope();
            if (!scope->hasVariableBinding(name))
            {
                scope->addVariableBinding(name, parameterType, tokens.getSourceLocation(parameterNode->nameToken()), tokens.source());
            }
            else
            {
                m_diagnostics.AddDuplicateParameterDeclarationError(
                    tokens.source(),
                    tokens.getSourceLocation(parameterNode->nameToken()),
                    name,
                    scope->tryGetVariableBindingSource(name),
                    scope->tryGetVariableBindingLocation(name));
            }
            parameterNode->setType(parameterType);
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
                m_diagnostics.AddReferenceReturnTypeError(
                    tokens.source(),
                    returnTypeNode->sourceLocation(tokens),
                    returnTypeNode->name());
            }
            types.push_back(returnType);
        }
        return types;
    }

    std::vector<Type> TypeChecker::typeCheckArgumentsNode(ArgumentsNode* argumentsNode, const TokenBuffer& tokens)
    {
        std::vector<Type> types{};
        for (const auto& argument : argumentsNode->arguments())
        {
            auto argumentType = typeCheckExpression(argument.get(), tokens);
            types.push_back(argumentType);
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

        const auto parsedValue = TryParseI32Literal(literal->literalLexeme());
        if (!parsedValue.has_value())
        {
            m_diagnostics.AddNumberLiteralOutOfRangeError(
                tokens.source(),
                literal->sourceLocation(tokens),
                literal->literalLexeme(),
                FormatTypeName(m_module, Type::I32()));
            return 0;
        }

        return parsedValue.value();
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

    const TokenBuffer& TypeChecker::tokensFor(const Statement* statement) const
    {
        const auto it = m_statementTokens.find(statement);
        if (it == m_statementTokens.end())
        {
            TODO("Missing token buffer for statement");
        }

        return *it->second;
    }

    void TypeChecker::typeCheckBlockNode(BlockNode* body, const TokenBuffer& tokens)
    {
        for (const auto& statement : body->statements())
        {
            typeCheckStatement(statement.get(), tokens);
        }
    }

    void TypeChecker::pushScope(ScopeKind kind)
    {
        auto parent = m_scopes.back().get();
        m_scopes.emplace_back(std::make_unique<Scope>(parent, kind));
    }

    void TypeChecker::popScope()
    {
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
