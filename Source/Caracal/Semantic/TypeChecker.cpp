#include "TypeChecker.h"

namespace Caracal
{
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
        collectDeclarations();
        collectMethodDeclarations();

        typeCheckFunctionSignatures();
        typeCheckTypeSignatures();
        typeCheckGlobalConstants();
        typeCheckTypeFieldDefinitions();
        typeCheckEnumDefinitions();
        typeCheckFunctionDefinitions();
        typeCheckTypeMethodDefinitions();
        
        return true;
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
                        m_globalConstantDeclarations.push_back(constantDeclaration);

                        break;
                    }
                    case NodeKind::EnumDefinitionStatement:
                    {
                        auto* enumStatement = static_cast<EnumDefinitionStatement*>(statement.get());
                        const auto& enumName = enumStatement->name();
                        // TODO check if type with same name exists already
                        auto& enumDefinition = m_module.createEnum(enumName, enumStatement);
                        enumStatement->setType(enumDefinition.type());
                        m_enumDeclarations.push_back(enumStatement);

                        break;
                    }
                    case NodeKind::TypeDefinitionStatement:
                    {
                        auto* typeStatement = static_cast<TypeDefinitionStatement*>(statement.get());
                        // TODO check if type with same name exists already
                        auto& typeDefinition = m_module.createType(typeStatement->name(), typeStatement);
                        typeStatement->setType(typeDefinition.type());
                        m_typeDeclarations.push_back(typeStatement);

                        break;
                    }
                    case NodeKind::FunctionDefinitionStatement:
                    {
                        auto* functionStatement = static_cast<FunctionDefinitionStatement*>(statement.get());
                        const auto& functionName = functionStatement->name();

                        std::vector<Parameter> parameters{};
                        const auto& parametersNodes = functionStatement->parametersNode()->parameters();
                        for (const auto& parameterNode : parametersNodes)
                        {
                            parameters.emplace_back(parameterNode->name(), Type::Undefined());
                        }

                        // TODO check if type with same name exists already
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
            auto typeType = m_module.tryGetTypeByName(typeDefinitionStatement->name());
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
                auto modifier = methodStatement->modifier();
                const auto& methodName = methodStatement->methodNameNode()->methodName();

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

            if (typeDefinition.tryGetMethodTypeByName("new") != Type::Undefined())
            {
                TODO("Add diagnostics for duplicate constructor declaration");
                continue;
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
            typeCheckFunctionSignature(const_cast<FunctionDefinitionStatement*>(functionDefinitionStatement));
        }
    }

    void TypeChecker::typeCheckTypeSignatures()
    {
        for (const auto* typeDefinitionStatement : m_typeDeclarations)
        {
            typeCheckTypeSignature(const_cast<TypeDefinitionStatement*>(typeDefinitionStatement));
        }
    }

    void TypeChecker::typeCheckGlobalConstants()
    {
        for (const auto* constantDeclaration : m_globalConstantDeclarations)
        {
            typeCheckConstantDeclaration(const_cast<ConstantDeclaration*>(constantDeclaration));
        }
    }

    void TypeChecker::typeCheckFunctionDefinitions()
    {
        for (const auto* functionDefinitionStatement : m_functionDeclarations)
        {
            typeCheckFunctionDefinitionStatement(const_cast<FunctionDefinitionStatement*>(functionDefinitionStatement));
        }
    }

    void TypeChecker::typeCheckEnumDefinitions()
    {
        for (const auto* enumDefinitionStatement : m_enumDeclarations)
        {
            typeCheckEnumDefinitionStatement(const_cast<EnumDefinitionStatement*>(enumDefinitionStatement));
        }
    }

    void TypeChecker::typeCheckTypeFieldDefinitions()
    {
        for (const auto* typeDefinitionStatement : m_typeDeclarations)
        {
            typeCheckTypeFieldDefinition(const_cast<TypeDefinitionStatement*>(typeDefinitionStatement));
        }
    }

    void TypeChecker::typeCheckTypeMethodDefinitions()
    {
        for (const auto* typeDefinitionStatement : m_typeDeclarations)
        {
            typeCheckTypeMethodDefinition(const_cast<TypeDefinitionStatement*>(typeDefinitionStatement));
        }
    }

    void TypeChecker::typeCheckFunctionSignature(FunctionDefinitionStatement* statement)
    {
        pushScope(ScopeKind::Function);
        auto parameters = typeCheckParametersNode(statement->parametersNode().get());
        auto returns = typeCheckReturnTypesNode(statement->returnTypesNode().get());
        popScope();

        const auto& functionName = statement->name();
        auto functionType = m_module.tryGetFunctionTypeByName(functionName);
        if (functionType == Type::Undefined())
        {
            TODO("This shouldn't happen");
        }

        auto& functionDefinition = m_module.getFunctionDefinition(functionType);
        auto isVariadic = !parameters.empty() && parameters.back().type() == Type::CVariadic();
        if (isVariadic && !statement->isExtern())
        {
            TODO("Add diagnostics for non-extern variadic function");
        }
        functionDefinition.setParameters(parameters);
        functionDefinition.setReturnTypes(returns);
        functionDefinition.setIsVariadic(isVariadic);
    }

    void TypeChecker::typeCheckTypeSignature(TypeDefinitionStatement* statement)
    {
        auto typeType = m_module.tryGetTypeByName(statement->name());
        if (typeType == Type::Undefined())
        {
            TODO("This shouldn't happen");
        }

        auto& typeDefinition = m_module.getTypeDefinition(typeType);
        typeCheckConstructorSignature(statement, typeDefinition, typeType);

        const auto& bodyStatements = statement->bodyNode()->statements();
        for (const auto& bodyStatement : bodyStatements)
        {
            if (bodyStatement->kind() != NodeKind::MethodDefinitionStatement)
            {
                continue;
            }

            const auto* methodStatement = static_cast<const MethodDefinitionStatement*>(bodyStatement.get());
            typeCheckMethodSignature(methodStatement, typeDefinition, typeType);
        }
    }

    void TypeChecker::typeCheckTypeFieldDefinition(TypeDefinitionStatement* statement)
    {
        auto typeType = statement->type();
        if (typeType == Type::Undefined())
        {
            TODO("This shouldn't happen");
        }

        pushScope(ScopeKind::Type);

        if (statement->constructorParameters().has_value())
        {
            static_cast<void>(typeCheckParametersNode(statement->constructorParameters().value().get()));
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

            typeCheckTypeFieldDeclaration(typeDefinition, static_cast<TypeFieldDeclaration*>(definitionStatement.get()), fieldIndex);
            ++fieldIndex;
        }

        popScope();
    }

    void TypeChecker::typeCheckTypeMethodDefinition(TypeDefinitionStatement* statement)
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
            static_cast<void>(typeCheckParametersNode(statement->constructorParameters().value().get()));
        }

        auto& typeDefinition = m_module.getTypeDefinition(typeType);
        for (const auto& fieldDefinition : typeDefinition.fields())
        {
            currentScope()->addVariableBinding(fieldDefinition.name(), fieldDefinition.type());
        }

        const auto& definitionStatements = statement->bodyNode()->statements();
        for (auto& definitionStatement : definitionStatements)
        {
            if (definitionStatement->kind() != NodeKind::MethodDefinitionStatement)
            {
                continue;
            }

            typeCheckMethodDefinitionStatement(static_cast<MethodDefinitionStatement*>(definitionStatement.get()));
        }

        popScope();
        m_currentType = Type::Undefined();
    }

    void TypeChecker::typeCheckMethodSignature(const MethodDefinitionStatement* methodStatement, TypeDefinition& typeDefinition, Type typeType)
    {
        const auto& methodName = methodStatement->methodNameNode()->methodName();

        pushScope(ScopeKind::Method);
        auto parameters = typeCheckParametersNode(methodStatement->parametersNode().get());
        auto returns = typeCheckReturnTypesNode(methodStatement->returnTypesNode().get());
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

    void TypeChecker::typeCheckConstructorSignature(const TypeDefinitionStatement* typeDefinitionStatement, TypeDefinition& typeDefinition, Type typeType)
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
            auto declaredConstructorParameters = typeCheckParametersNode(typeDefinitionStatement->constructorParameters().value().get());
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

    void TypeChecker::typeCheckStatement(Statement* statement)
    {
        switch (statement->kind())
        {
            case NodeKind::ConstantDeclaration:
            {
                typeCheckConstantDeclaration(static_cast<ConstantDeclaration*>(statement));
                break;
            }
            case NodeKind::VariableDeclaration:
            {
                typeCheckVariableDeclaration(static_cast<VariableDeclaration*>(statement));
                break;
            }
            case NodeKind::ExpressionStatement:
            {
                typeCheckExpressionStatement(static_cast<ExpressionStatement*>(statement));
                break;
            }
            case NodeKind::AssignmentStatement:
            {
                typeCheckAssignmentStatement(static_cast<AssignmentStatement*>(statement));
                break;
            }
            case NodeKind::FunctionDefinitionStatement:
            {
                typeCheckFunctionDefinitionStatement(static_cast<FunctionDefinitionStatement*>(statement));
                break;
            }
            case NodeKind::EnumDefinitionStatement:
            {
                typeCheckEnumDefinitionStatement(static_cast<EnumDefinitionStatement*>(statement));
                break;
            }
            case NodeKind::IfStatement:
            {
                typeCheckIfStatement(static_cast<IfStatement*>(statement));
                break;
            }
            case NodeKind::WhileStatement:
            {
                typeCheckWhileStatement(static_cast<WhileStatement*>(statement));
                break;
            }
            case NodeKind::ReturnStatement:
            {
                typeCheckReturnStatement(static_cast<ReturnStatement*>(statement));
                break;
            }
            case NodeKind::BlockNode:
            {
                typeCheckBlockNode(static_cast<BlockNode*>(statement));
                break;
            }
            default:
            {
                break;
            }
        }
    }

    void TypeChecker::typeCheckConstantDeclaration(ConstantDeclaration* statement)
    {
        auto rightExpression = statement->rightExpression().get();
        auto rightType = typeCheckExpression(rightExpression);

        auto leftExpression = statement->leftExpression().get();
        if (leftExpression->kind() == NodeKind::NameExpression)
        {
            auto nameExpression = static_cast<NameExpression*>(leftExpression);
            const auto& name = nameExpression->name();
            auto scope = currentScope();
            if (!scope->hasVariableBinding(name))
            {
                scope->addVariableBinding(name, rightType);

                if (statement->isGlobalConstant())
                {
                    // TODO maybe remove the return value from the function?
                    static_cast<void>(m_module.createConstant(name, rightExpression));
                }
            }
            else
            {
                TODO("Add error diagnostics for duplicate constant declaration");
            }
            nameExpression->setType(rightType);
        }

        if (statement->explicitType().has_value())
        {
            auto explicitType = typeCheckTypeNameNode(statement->explicitType().value().get());
            if (rightType != explicitType)
            {
                TODO("Type mismatch error diagnostics");
            }
        }

        statement->setType(rightType);
    }

    void TypeChecker::typeCheckVariableDeclaration(VariableDeclaration* statement)
    {
        auto rightType = typeCheckExpression(statement->rightExpression().get());

        auto leftExpression = statement->leftExpression().get();
        if (leftExpression->kind() == NodeKind::NameExpression)
        {
            auto nameExpression = static_cast<NameExpression*>(leftExpression);
            const auto& name = nameExpression->name();
            auto scope = currentScope();
            if (!scope->hasVariableBinding(name))
            {
                scope->addVariableBinding(name, rightType);
            }
            else
            {
                TODO("Add error diagnostics for duplicate variable declaration");
            }
            nameExpression->setType(rightType);
        }

        if (statement->explicitType().has_value())
        {
            auto explicitType = typeCheckTypeNameNode(statement->explicitType().value().get());
            if (rightType != explicitType)
            {
                TODO("Type mismatch error diagnostics");
            }
        }

        statement->setType(rightType);
    }

    void TypeChecker::typeCheckExpressionStatement(ExpressionStatement* statement)
    {
        auto expressionType = typeCheckExpression(statement->expression().get());
        statement->setType(expressionType);
    }

    void TypeChecker::typeCheckAssignmentStatement(AssignmentStatement* statement)
    {
        auto leftType = typeCheckExpression(statement->leftExpression().get());
        if (leftType.isReference())
        {
            leftType = leftType.toValue();
        }
        auto rightType = typeCheckExpression(statement->rightExpression().get());
        if (rightType.isReference())
        {
            rightType = rightType.toValue();
        }

        if (leftType == Type::Discard())
            return;

        if (leftType != rightType)
        {
            TODO("Add error diagnostics for type mismatch in assignment");
        }
    }

    void TypeChecker::typeCheckFunctionDefinitionStatement(FunctionDefinitionStatement* statement)
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
        for (const auto& parameter : parameters)
        {
            currentScope()->addVariableBinding(parameter.name(), parameter.type());
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

        typeCheckBlockNode(statement->bodyNode().get());

        popScope();
        m_currentReturnType = Type::Void();
    }

    void TypeChecker::typeCheckEnumDefinitionStatement(EnumDefinitionStatement* statement)
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
        const auto isFlag = statement->isFlag();
        auto currentFieldValue = isFlag ? 1 : 0;
        auto step = 1;

        if (statement->baseType().has_value())
        {
            baseType = typeCheckTypeNameNode(statement->baseType().value().get());
            enumDefinition.setBaseType(baseType);
        }

        if (statement->hasStep())
        {
            auto stepAnnotation = statement->annotation().value().get();
            auto arguments = stepAnnotation->argumentsNode().value().get();
            auto stepParameter = arguments->arguments().at(0).get();
            if (stepParameter->kind() == NodeKind::NumberLiteral)
            {
                step = convertToI32(static_cast<NumberLiteral*>(stepParameter));
            }
        }

        const auto& fieldNodes = statement->fieldNodes();
        for (auto& fieldNode : fieldNodes)
        {
            const auto& fieldName = fieldNode->name();
            if (enumDefinition.hasField(fieldName))
            {
                TODO("Add error diagnostics for duplicate enum field name");
            }

            if (isFlag)
            {
                if (fieldNode->valueExpression().has_value())
                {
                    TODO("Add error diagnostics for flag enums not allowing explicit values");
                }

                enumDefinition.addField(fieldName, currentFieldValue);
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
                auto fieldValueType = typeCheckExpression(expression);
                if (baseType == Type::Undefined())
                {
                    baseType = fieldValueType;
                    enumDefinition.setBaseType(baseType);
                }
                else if (fieldValueType != baseType)
                {
                    TODO("Add error diagnostics for enum field value type mismatch");
                }

                if (expression->kind() == NodeKind::NumberLiteral && expression->type() == Type::I32())
                {
                    auto value = convertToI32(static_cast<NumberLiteral*>(expression));
                    enumDefinition.addField(fieldName, value);
                    fieldNode->setValue(value);
                    currentFieldValue = value + step;
                }
                else
                {
                    enumDefinition.addField(fieldName, expression);
                }
            }
            else
            {
                enumDefinition.addField(fieldName, currentFieldValue);
                fieldNode->setValue(currentFieldValue);
                currentFieldValue += step;
            }
        }

        if (enumDefinition.baseType() == Type::Undefined())
        {
            enumDefinition.setBaseType(defaultBaseType);
        }
    }

    void TypeChecker::typeCheckTypeFieldDeclaration(TypeDefinition& typeDefinition, TypeFieldDeclaration* statement, i32 fieldIndex)
    {
        const auto& fieldName = statement->nameExpression()->name();
        if (typeDefinition.tryGetFieldByName(fieldName).type() != Type::Undefined())
        {
            TODO("Add error diagnostics for duplicate type field declaration");
            return;
        }

        auto fieldType = Type::Undefined();
        Expression* fieldExpression = nullptr;
        if (statement->explicitType().has_value())
        {
            fieldType = typeCheckTypeNameNode(statement->explicitType().value().get());
        }

        if (statement->rightExpression().has_value())
        {
            fieldExpression = statement->rightExpression().value().get();
            auto expressionType = typeCheckExpression(fieldExpression);
            if (fieldType == Type::Undefined())
            {
                fieldType = expressionType;
            }
            else if (fieldType != expressionType)
            {
                TODO("Type mismatch error diagnostics");
            }
        }

        if (fieldType == Type::Undefined())
        {
            TODO("Add error diagnostics for missing type field type");
            return;
        }

        statement->nameExpression()->setType(fieldType);
        statement->setType(fieldType);
        typeDefinition.addField(fieldType, fieldName, fieldIndex, fieldExpression);
    }

    void TypeChecker::typeCheckMethodDefinitionStatement(MethodDefinitionStatement* statement)
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
        for (const auto& parameter : parameters)
        {
            currentScope()->addVariableBinding(parameter.name(), parameter.type());
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

        typeCheckBlockNode(statement->bodyNode().get());

        popScope();
        m_currentReturnType = Type::Void();
    }

    void TypeChecker::typeCheckIfStatement(IfStatement* statement)
    {
        auto conditionType = typeCheckExpression(statement->condition().get());
        conditionType = coerceConditionType(conditionType, statement->condition().get());
        if (conditionType != Type::Bool())
        {
            TODO("Add an error because only bool is allowed");
        }

        typeCheckStatement(statement->trueStatement().get());

        if (statement->hasFalseBlock())
        {
            typeCheckStatement(statement->falseStatement().value().get());
        }
    }

    void TypeChecker::typeCheckWhileStatement(WhileStatement* statement)
    {
        auto conditionType = typeCheckExpression(statement->condition().get());
        conditionType = coerceConditionType(conditionType, statement->condition().get());
        if (conditionType != Type::Bool())
        {
            TODO("Add an error because only bool is allowed");
        }

        typeCheckStatement(statement->trueStatement().get());
    }

    void TypeChecker::typeCheckReturnStatement(ReturnStatement* statement)
    {
        if (statement->expression().has_value())
        {
            auto type = typeCheckExpression(statement->expression().value().get());
            if (m_currentReturnType != Type::Void() && m_currentReturnType != type)
            {
                TODO("this isnt correct when the function has multiple returns but works for now");
            }
            statement->setType(type);
            m_currentReturnType = type;
        }
        else
        {
            statement->setType(Type::Void());
        }
    }

    Type TypeChecker::typeCheckExpression(Expression* expression)
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
                return typeCheckNumberLiteral(static_cast<NumberLiteral*>(expression));
            }
            case NodeKind::GroupingExpression:
            {
                return typeCheckGroupingExpression(static_cast<GroupingExpression*>(expression));
            }
            case NodeKind::UnaryExpression:
            {
                return typeCheckUnaryExpressionExpression(static_cast<UnaryExpression*>(expression));
            }
            case NodeKind::BinaryExpression:
            {
                return typeCheckBinaryExpressionExpression(static_cast<BinaryExpression*>(expression));
            }
            case NodeKind::NameExpression:
            {
                return typeCheckNameExpression(static_cast<NameExpression*>(expression));
            }
            case NodeKind::FunctionCallExpression:
            {
                return typeCheckFunctionCallExpression(static_cast<FunctionCallExpression*>(expression));
            }
            case NodeKind::MemberAccessExpression:
            {
                return typeCheckMemberAccessExpression(static_cast<MemberAccessExpression*>(expression));
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

    Type TypeChecker::typeCheckGroupingExpression(GroupingExpression* groupingExpression)
    {
        auto type = typeCheckExpression(groupingExpression->expression().get());

        groupingExpression->setType(type);
        return type;
    }

    Type TypeChecker::typeCheckUnaryExpressionExpression(UnaryExpression* unaryExpression)
    {
        switch (unaryExpression->unaryOperator())
        {
            case UnaryOperatorKind::LogicalNegation:
            case UnaryOperatorKind::ValueNegation:
            {
                auto type = typeCheckExpression(unaryExpression->expression().get());

                unaryExpression->setType(type);
                return type;
            }
            case UnaryOperatorKind::ReferenceOf:
            {
                auto type = typeCheckExpression(unaryExpression->expression().get());
                if (type.isReference())
                {
                    TODO("Add error diagnostics for already being a reference");
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

    Type TypeChecker::typeCheckBinaryExpressionExpression(BinaryExpression* binaryExpression)
    {
        switch (binaryExpression->binaryOperator())
        {
            case BinaryOperatorKind::MemberAccess:
            {
                auto leftType = typeCheckExpression(binaryExpression->leftExpression().get());
                if (leftType.kind() == TypeKind::Enum)
                {
                    binaryExpression->setType(leftType);
                    binaryExpression->rightExpression()->setType(leftType);
                    return leftType;
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
                            if (!typeCheckCallArguments(functionCallExpression, methodDefinition))
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
                            TODO("Add error diagnostics for unknown method");
                        }
                    }
                    else if (binaryExpression->rightExpression()->kind() == NodeKind::NameExpression)
                    {
                        auto* fieldNameExpression = static_cast<NameExpression*>(binaryExpression->rightExpression().get());
                        const auto& fieldDefinition = typeDefinition.tryGetFieldByName(fieldNameExpression->name());
                        if (fieldDefinition.type() == Type::Undefined())
                        {
                            TODO("Add error diagnostics for unknown field");
                            return Type::Undefined();
                        }

                        auto fieldType = fieldDefinition.type();
                        fieldNameExpression->setType(fieldType);
                        binaryExpression->setType(fieldType);
                        return fieldType;
                    }
                    else
                    {
                        TODO("Add error diagnostics for invalid member access on type");
                    }
                }

                TODO("Handle MemberAccess");
                return Type::Undefined();
            }
            case BinaryOperatorKind::Addition:
            case BinaryOperatorKind::Subtraction:
            case BinaryOperatorKind::Multiplication:
            case BinaryOperatorKind::Division:
            {
                auto leftType = typeCheckExpression(binaryExpression->leftExpression().get());
                if (leftType.isReference())
                {
                    leftType = leftType.toValue();
                }
                auto rightType = typeCheckExpression(binaryExpression->rightExpression().get());
                if (rightType.isReference())
                {
                    rightType = rightType.toValue();
                }

                // TODO we need to be able look up the resulting type for a binary expression, 
                // for now we'll just make sure left and right have the same type and use that one
                if (leftType != rightType)
                {
                    TODO("Type mismatch error diagnostics");
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
                auto leftType = typeCheckExpression(binaryExpression->leftExpression().get());
                if (leftType.isReference())
                {
                    leftType = leftType.toValue();
                }
                auto rightType = typeCheckExpression(binaryExpression->rightExpression().get());
                if (rightType.isReference())
                {
                    rightType = rightType.toValue();
                }

                if (!areComparableTypes(leftType, rightType))
                {
                    TODO("Type mismatch error diagnostics");
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

    Type TypeChecker::typeCheckNameExpression(NameExpression* expression)
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

        TODO("Add error diagnostics for unknown name expression");

        return Type::Undefined();
    }

    Type TypeChecker::typeCheckFunctionCallExpression(FunctionCallExpression* functionCallExpression)
    {
        const auto& name = functionCallExpression->nameExpression()->name();
        auto functionType = m_module.tryGetFunctionTypeByName(name);
        if (functionType == Type::Undefined())
        {
            TODO("Add error diagnostics for unknown function call");
            return Type::Undefined();
        }
        const auto& functionDefinition = m_module.getFunctionDefinition(functionType);

        if (!typeCheckCallArguments(functionCallExpression, functionDefinition))
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

    Type TypeChecker::typeCheckMemberAccessExpression(MemberAccessExpression* memberAccessExpression)
    {
        if (m_currentType == Type::Undefined())
        {
            TODO("Member access expression only supported in type scope");
            return Type::Undefined();
        }

        auto& typeDefinition = m_module.getTypeDefinition(m_currentType);
        auto type = typeCheckExpression(memberAccessExpression->expression().get());
        memberAccessExpression->setType(type);

        return type;
    }

    bool TypeChecker::typeCheckCallArguments(
        FunctionCallExpression* functionCallExpression,
        const FunctionDefinition& functionDefinition)
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

        if (isVariadic)
        {
            if (arguments.size() < expectedArgumentCount)
            {
                TODO("Add error diagnostics for argument count mismatch in variadic function call");
                return false;
            }
        }
        else
        {
            if (arguments.size() != expectedArgumentCount)
            {
                TODO("Add error diagnostics for argument count mismatch");
                return false;
            }
        }

        for (size_t i = 0; i < arguments.size(); ++i)
        {
            auto* argument = arguments[i].get();
            auto argumentType = typeCheckExpression(argument);

            if (i < expectedArgumentCount)
            {
                const auto expectedType = parameterTypes[i + parameterOffset].type();
                if (argumentType != expectedType)
                {
                    TODO("Add error diagnostics for argument type mismatch");
                    return false;
                }
            }
            else
            {
                if (argumentType == Type::Undefined() || argumentType == Type::Void())
                {
                    TODO("Add error diagnostics for invalid variadic argument type");
                    return false;
                }
            }
        }

        return true;
    }

    Type TypeChecker::typeCheckNumberLiteral(NumberLiteral* literal)
    {
        auto numberType = Type::Undefined();
        if (literal->explicitType().has_value())
        {
            numberType = typeCheckTypeNameNode(literal->explicitType().value().get());
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

            // TODO check if the value fits into the type
            // TODO if it doesnt fit, then we need to print diagnostics 
        }

        literal->setType(numberType);
        return numberType;
    }

    Type TypeChecker::typeCheckTypeNameNode(TypeNameNode* typeNameNode)
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
            TODO("Add error diagnostics for unknown type name");
            return Type::Undefined();
        }
    }

    std::vector<Parameter> TypeChecker::typeCheckParametersNode(ParametersNode* parametersNode)
    {
        std::vector<Parameter> parameters{};
        for (const auto& parameterNode : parametersNode->parameters())
        {
            auto parameterType = typeCheckTypeNameNode(parameterNode->typeName().get());
            parameters.push_back(Parameter{ parameterNode->name(), parameterType });

            // register parameter in current scope
            const auto& name = parameterNode->name();
            auto scope = currentScope();
            if (!scope->hasVariableBinding(name))
            {
                scope->addVariableBinding(name, parameterType);
            }
            else
            {
                TODO("Add error diagnostics for duplicate parameter declaration");
            }
            parameterNode->setType(parameterType);
        }
        return parameters;
    }

    std::vector<Type> TypeChecker::typeCheckReturnTypesNode(ReturnTypesNode* returnTypesNode)
    {
        std::vector<Type> types{};
        for (const auto& returnTypeNode : returnTypesNode->returnTypes())
        {
            auto returnType = typeCheckTypeNameNode(returnTypeNode.get());
            if (returnType.isReference())
            {
                TODO("Add error diagnostics for return type cannot be a reference");
            }
            types.push_back(returnType);
        }
        return types;
    }

    std::vector<Type> TypeChecker::typeCheckArgumentsNode(ArgumentsNode* argumentsNode)
    {
        std::vector<Type> types{};
        for (const auto& argument : argumentsNode->arguments())
        {
            auto argumentType = typeCheckExpression(argument.get());
            types.push_back(argumentType);
        }
        return types;
    }

    i32 TypeChecker::convertToI32(NumberLiteral* literal)
    {
        auto literalType = typeCheckNumberLiteral(literal);
        if (literalType != Type::I32())
        {
            TODO("Add error diagnostics for number literal type mismatch");
            return 0;
        }

        const auto& lexeme = literal->literalLexeme();
        auto value = std::stoll(lexeme);

        return static_cast<i32>(value);
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

    void TypeChecker::typeCheckBlockNode(BlockNode* body)
    {
        for (const auto& statement : body->statements())
        {
            typeCheckStatement(statement.get());
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
