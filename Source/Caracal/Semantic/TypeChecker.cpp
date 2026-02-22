#include "TypeChecker.h"

namespace Caracal
{
    bool typeCheck(
        ParseTree& parseTree,
        const TypeCheckerOptions& options,
        TypeDatabase& typeDatabase,
        DiagnosticsBag& diagnostics) noexcept
    {
        TypeChecker typeChecker{ parseTree, options, typeDatabase, diagnostics };
        return typeChecker.typeCheck();
    }

    TypeChecker::TypeChecker(
        ParseTree& parseTree,
        const TypeCheckerOptions& options,
        TypeDatabase& typeDatabase,
        DiagnosticsBag& diagnostics)
        : m_parseTree{ parseTree }
        , m_options{ options }
        , m_typeDatabase{ typeDatabase }
        , m_diagnostics{ diagnostics }
        , m_currentReturnType{ Type::Void() }
        , m_scopes{}
    {
        m_scopes.emplace_back(std::make_unique<Scope>(nullptr, ScopeKind::Global));
    }

    bool TypeChecker::typeCheck()
    {
        for (const auto& globalStatement : m_parseTree.statements())
        {
            typeCheckStatement(globalStatement.get());
        }
        return true;
    }

    void TypeChecker::typeCheckStatement(Statement* statement)
    {
        switch (statement->kind())
        {
            case NodeKind::ConstantDeclaration:
            {
                typeCheckConstantDeclaration((ConstantDeclaration*)statement);
                break;
            }
            case NodeKind::VariableDeclaration:
            {
                typeCheckVariableDeclaration((VariableDeclaration*)statement);
                break;
            }
            case NodeKind::ExpressionStatement:
            {
                typeCheckExpressionStatement((ExpressionStatement*)statement);
                break;
            }
            case NodeKind::AssignmentStatement:
            {
                typeCheckAssignmentStatement((AssignmentStatement*)statement);
                break;
            }
            case NodeKind::FunctionDefinitionStatement:
            {
                typeCheckFunctionDefinitionStatement((FunctionDefinitionStatement*)statement);
                break;
            }
            case NodeKind::EnumDefinitionStatement:
            {
                typeCheckEnumDefinitionStatement((EnumDefinitionStatement*)statement);
                break;
            }
            case NodeKind::IfStatement:
            {
                typeCheckIfStatement((IfStatement*)statement);
                break;
            }
            case NodeKind::WhileStatement:
            {
                typeCheckWhileStatement((WhileStatement*)statement);
                break;
            }
            case NodeKind::ReturnStatement:
            {
                typeCheckReturnStatement((ReturnStatement*)statement);
                break;
            }
            case NodeKind::BlockNode:
            {
                typeCheckBlockNode((BlockNode*)statement);
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
        auto rightType = typeCheckExpression(statement->rightExpression().get());

        auto leftExpression = statement->leftExpression().get();
        if(leftExpression->kind() == NodeKind::NameExpression)
        {
            auto nameExpression = (NameExpression*)leftExpression;
            const auto& name = nameExpression->name();
            auto scope = currentScope();
            if(!scope->hasVariableBinding(name))
            {
                scope->addVariableBinding(name, rightType);
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
            if(rightType != explicitType)
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
            auto nameExpression = (NameExpression*)leftExpression;
            auto nameToken = nameExpression->nameToken();
            auto nameLexeme = m_parseTree.tokens().getLexeme(nameToken);
            auto scope = currentScope();
            if (!scope->hasVariableBinding(nameLexeme))
            {
                scope->addVariableBinding(nameLexeme, rightType);
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
        auto rightType = typeCheckExpression(statement->rightExpression().get());

        if(leftType == Type::Discard())
            return;

        if(leftType != rightType)
        {
            TODO("Add error diagnostics for type mismatch in assignment");
        }
    }
    
    void TypeChecker::typeCheckFunctionDefinitionStatement(FunctionDefinitionStatement* statement)
    {
        m_currentReturnType = Type::Void();
        auto parentScope = currentScope();
        pushScope(ScopeKind::Function);
    
        // TODO check if function with same name and parameters exists already

        auto functionName = statement->name();
        auto parametersTypes = typeCheckParametersNode(statement->parametersNode().get());
        auto returnTypes = typeCheckReturnTypesNode(statement->returnTypesNode().get());

        auto& functionDefinition = m_typeDatabase.createFunction(functionName, parametersTypes, returnTypes);
        auto functionType = functionDefinition.type();

        typeCheckBlockNode(statement->bodyNode().get());
        
        // TODO check if return type matches declared return types

        statement->setType(functionType);
    
        popScope();
        m_currentReturnType = Type::Void();
    }

    void TypeChecker::typeCheckEnumDefinitionStatement(EnumDefinitionStatement* statement)
    {
        const auto& enumName = statement->name();
        auto baseType = Type::Undefined();
        auto defaultBaseType = m_options.defaultEnumBaseType;
        auto& enumDefinition = m_typeDatabase.createEnum(enumName);
        auto enumType = enumDefinition.type();
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
                step = convertToI32((NumberLiteral*)stepParameter);
            }
        }

        const auto& fieldNodes = statement->fieldNodes();
        i32 currentFieldValue = 0;
        for(auto& fieldNode : fieldNodes)
        {
            const auto& fieldName = fieldNode->name();
            if(enumDefinition.hasField(fieldName))
            {
                TODO("Add error diagnostics for duplicate enum field name");
            }

            if(fieldNode->valueExpression().has_value())
            {
                auto expression = fieldNode->valueExpression().value().get();
                auto fieldValueType = typeCheckExpression(expression);
                if(baseType == Type::Undefined())
                {
                    baseType = fieldValueType;
                    enumDefinition.setBaseType(baseType);
                }
                else if(fieldValueType != baseType)
                {
                    TODO("Add error diagnostics for enum field value type mismatch");
                }

                if(expression->kind() == NodeKind::NumberLiteral && expression->type() == Type::I32())
                {
                    auto value = convertToI32((NumberLiteral*)expression);
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

        statement->setType(enumType);
    }

    void TypeChecker::typeCheckIfStatement(IfStatement* statement)
    {
        auto conditionType = typeCheckExpression(statement->condition().get());
        if (conditionType != Type::Bool())
        {
            TODO("Add an error because only bool is allowed");
        }
     
        typeCheckStatement(statement->trueStatement().get());
    
        if(statement->hasFalseBlock())
        {
            typeCheckStatement(statement->falseStatement().value().get());
        }
    }

    void TypeChecker::typeCheckWhileStatement(WhileStatement* statement)
    {
        auto conditionType = typeCheckExpression(statement->condition().get());
        if (conditionType != Type::Bool())
        {
            TODO("Add an error because only bool is allowed");
        }

        typeCheckStatement(statement->trueStatement().get());
    }

    void TypeChecker::typeCheckReturnStatement(ReturnStatement* statement)
    {
        if(statement->expression().has_value())
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
                return typeCheckNumberLiteral((NumberLiteral*)expression);
            }
            case NodeKind::GroupingExpression:
            {
                return typeCheckGroupingExpression((GroupingExpression*)expression);
            }
            case NodeKind::UnaryExpression:
            {
                return typeCheckUnaryExpressionExpression((UnaryExpression*)expression);
            }
            case NodeKind::BinaryExpression:
            {
                return typeCheckBinaryExpressionExpression((BinaryExpression*)expression);
            }
            case NodeKind::NameExpression:
            {
                return typeCheckNameExpression((NameExpression*)expression);
            }
            case NodeKind::FunctionCallExpression:
            {
                return typeCheckFunctionCallExpression((FunctionCallExpression*)expression);
            }
            /*
            case NodeKind::MemberAccessExpression:
            {
                return typeCheckMemberAccessExpression((MemberAccessExpression*)expression);
            }
            case NodeKind::DiscardLiteral:
            {
                return typeCheckDiscardLiteral((DiscardLiteral*)expression);
            }*/
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
                TODO("Handle reference of operator");
                return Type::Undefined();
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
                if(leftType.kind() == TypeKind::Enum)
                {
                    binaryExpression->setType(leftType);
                    binaryExpression->rightExpression()->setType(leftType);
                    return leftType;
                }

                TODO("Handle MemberAccess");
                //auto rightType = typeCheckExpression(binaryExpression->rightExpression().get());
                
                return Type::Undefined();
            }
            case BinaryOperatorKind::Addition:
            case BinaryOperatorKind::Subtraction:
            case BinaryOperatorKind::Multiplication:
            case BinaryOperatorKind::Division:
            {
                auto leftType = typeCheckExpression(binaryExpression->leftExpression().get());
                auto rightType = typeCheckExpression(binaryExpression->rightExpression().get());

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
                auto rightType = typeCheckExpression(binaryExpression->rightExpression().get());

                // TODO we need to be able look up the resulting type for a binary expression,
                // for now we'll just make sure left and right have the same type and use that one
                if (leftType != rightType)
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

        auto optionalType = m_typeDatabase.tryGetTypeByName(name);
        if (optionalType.has_value())
        {
            auto type = optionalType.value();
            expression->setType(type);
            return type;
        }

        TODO("Add error diagnostics for unknown name expression");

        return Type::Undefined();
    }
    
    Type TypeChecker::typeCheckFunctionCallExpression(FunctionCallExpression* functionCallExpression)
    {
        const auto& name = functionCallExpression->nameExpression()->nameToken();
        auto lexeme = m_parseTree.tokens().getLexeme(name);
        auto functionType = m_typeDatabase.tryGetFunctionTypeByName(lexeme);
        if(functionType == Type::Undefined())
        {
            TODO("Add error diagnostics for unknown function call");
            return Type::Undefined();
        }
        const auto& functionDefinition = m_typeDatabase.getFunctionDefinition(functionType);

        auto argumentsNode = functionCallExpression->argumentsNode().get();
        auto argumentTypes = typeCheckArgumentsNode(argumentsNode);

        const auto& parameterTypes = functionDefinition.parameters();
        auto isVariadic = functionDefinition.isVariadic();
        auto parameterCount = (isVariadic ? parameterTypes.size() - 1 : parameterTypes.size());
        if (isVariadic)
        {
            // we can either have same size or one less argument
            if (parameterTypes.size() != argumentTypes.size() 
                && parameterTypes.size() - 1 != argumentTypes.size())
            {
                TODO("Add error diagnostics for argument count mismatch in variadic function call");
                return Type::Undefined();
            }
        }
        else
        {
            if (parameterTypes.size() != argumentTypes.size())
            {
                TODO("Add error diagnostics for argument count mismatch");
                return Type::Undefined();
            }
        }

        for (size_t i = 0; i < parameterCount; ++i)
        {
            if (parameterTypes[i].type() != argumentTypes[i])
            {
                TODO("Add error diagnostics for argument type mismatch");
                return Type::Undefined();
            }
        }

        const auto& returnTypes = functionDefinition.returnTypes();
        Type returnType = Type::Void();
        if(returnTypes.size() == 1)
        {
            returnType = returnTypes[0];
        }
        else if(returnTypes.size() > 1)
        {
            TODO("Handle multiple return types");
        }

        functionCallExpression->setFunctionType(functionType);
        functionCallExpression->setType(returnType);
        return returnType;
    }

    Type TypeChecker::typeCheckNumberLiteral(NumberLiteral* literal)
    {
        auto numberType = Type::Undefined();
        if(literal->explicitType().has_value())
        {
            numberType = typeCheckTypeNameNode(literal->explicitType().value().get());
        }
        else
        {
            const auto& literalToken = literal->literalToken();
            const auto lexeme = m_parseTree.tokens().getLexeme(literalToken);
            
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
        auto type = TypeDatabase::TryFindBuiltin(name);
        
        // TODO ref and nullable handling
        typeNameNode->setType(type);
        return type;
    }
    
    std::vector<Parameter> TypeChecker::typeCheckParametersNode(ParametersNode* parametersNode)
    {
        std::vector<Parameter> parameters{};
        for(const auto& parameterNode : parametersNode->parameters())
        {
            auto parameterType = typeCheckTypeNameNode(parameterNode->typeName().get());
            parameters.push_back(Parameter{ parameterNode->name(), parameterType});

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
        for(const auto& returnTypeNode : returnTypesNode->returnTypes())
        {
            auto returnType = typeCheckTypeNameNode(returnTypeNode.get());
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

        const auto& literalToken = literal->literalToken();
        const auto lexeme = m_parseTree.tokens().getLexeme(literalToken);
        auto value = std::stoll(std::string(lexeme));

        return static_cast<i32>(value);
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
        if(m_scopes.size() == 0)
        {
            TODO("Popped too many scopes");
        }
    }
    
    Scope* TypeChecker::currentScope() const noexcept
    {
        return m_scopes.back().get();
    }
}

//TypedStatement* TypeChecker::typeCheckTypeDefinitionStatement(TypeDefinitionStatement* statement)
//{
//    auto& nameToken = statement->name();
//    auto typeName = m_parseTree.tokens().getLexeme(nameToken);
//    // TODO check if there is already a type with the name
//    // TODO create all type variations
//    auto refName = QString("ref ") + typeName.toString();
//    auto newRefType = m_typeDatabase.createType(refName, TypeKind::Type);
//    auto newType = m_typeDatabase.createType(typeName, TypeKind::Type);
//    currentScope()->addTypeBinding(typeName, newType);
//
//    pushScope(ScopeKind::Type);
//    auto typeFields = typeCheckTypeFieldDefinitionNodes(newType, statement->body());
//    auto typedMethods = typeCheckTypeMethodDefinitions(newRefType, newType, statement->body());
//    popScope();
//
//    return new TypedTypeDefinitionStatement(typeName, newType, typeFields, typedMethods, statement);
//}
//
//TypedMethodDefinitionStatement* TypeChecker::typeCheckTypeMethodDefinitionStatement(Type newRefType, Type newType, MethodDefinitionStatement* statement)
//{
//    auto parentScope = currentScope();
//    pushScope(ScopeKind::Method);
//
//    auto& nameToken = statement->name();
//    auto methodName = m_parseTree.tokens().getLexeme(nameToken);
//    // TODO check if method with same name and parameters exists already
//    auto newMethodType = m_typeDatabase.createFunction(methodName);
//    parentScope->addFunctionBinding(methodName, newMethodType);
//    auto& typeDefinition = m_typeDatabase.getTypeDefinition(newType);
//    typeDefinition.addFunction(newMethodType, methodName);
//    auto& methodDefinition = typeDefinition.getFunctionDefinition(newMethodType);
//
//    // TODO this is wrong but works for now, change to ref type once we register fields and methods in all type variants
//    currentScope()->addTypeBinding(QStringView(u"this"), newType);
//    // TODO add method to type
//    auto thisParameter = new Parameter(QStringView(u"this"), nullptr, newRefType);
//    auto parameters = typeCheckFunctionParameters(statement->parameters());
//    parameters.prepend(thisParameter);
//    methodDefinition.setParameters(parameters);
//
//    auto [typedBody, returnType] = typeCheckFunctionBodyNode(statement->body());
//    methodDefinition.setReturnType(returnType);
//
//    popScope();
//
//    return new TypedMethodDefinitionStatement(methodName, newType, newMethodType, parameters, returnType, typedBody, statement);
//}
//QList<TypedFieldDefinitionNode*> TypeChecker::typeCheckEnumFieldDefinitionNodes(
//    Type newType,
//    Type baseType,
//    const QList<EnumFieldDefinitionStatement*>& fieldDefinitions)
//{
//    auto& enumDefinition = m_typeDatabase.getEnumDefinition(newType);
//
//    QList<TypedFieldDefinitionNode*> enumFields;
//    int nextValue = 0;
//    for (const auto definition : fieldDefinitions)
//    {
//        auto& nameToken = definition->name()->identifier();
//        auto name = m_parseTree.tokens().getLexeme(nameToken);
//
//        if (definition->value().has_value())
//        {
//            auto numberLiteral = definition->value().value();
//            auto& numberToken = numberLiteral->token();
//            auto valueLexeme = m_parseTree.tokens().getLexeme(numberToken);
//
//            auto [typedLiteral, value] = convertValueToTypedLiteral(valueLexeme, baseType, definition);
//            if (typedLiteral != nullptr)
//            {
//                nextValue = value + 1;
//                enumFields.append(new TypedFieldDefinitionNode(name, baseType, typedLiteral));
//                enumDefinition.addField(newType, name, typedLiteral);
//            }
//        }
//        else
//        {
//            auto [typedLiteral, value] = convertValueToTypedLiteral(nextValue++, baseType, definition);
//            if (typedLiteral != nullptr)
//            {
//                enumFields.append(new TypedFieldDefinitionNode(name, baseType, typedLiteral));
//                enumDefinition.addField(newType, name, typedLiteral);
//            }
//        }
//    }
//    return enumFields;
//}
//
//QList<TypedFieldDefinitionNode*> TypeChecker::typeCheckTypeFieldDefinitionNodes(Type newType, BlockNode* body)
//{
//    auto& typeDefinition = m_typeDatabase.getTypeDefinition(newType);
//
//    QList<TypedFieldDefinitionNode*> typeFields;
//
//    for (const auto statement : body->statements())
//    {
//        if (statement->kind() != NodeKind::FieldDefinitionStatement)
//            continue;
//
//        auto fieldDeclaration = (FieldDefinitionStatement*)statement;
//        auto& nameToken = fieldDeclaration->name()->identifier();
//        auto name = m_parseTree.tokens().getLexeme(nameToken);
//
//        auto type = Type::Undefined();
//        if (fieldDeclaration->type().has_value())
//        {
//            auto& fieldTypeName = fieldDeclaration->type().value();
//            type = convertTypeNameToType(fieldTypeName);
//        }
//
//        TypedExpression* expression = nullptr;
//        if (fieldDeclaration->expression().has_value())
//        {
//            auto fieldExpression = fieldDeclaration->expression().value();
//            expression = typeCheckExpression(fieldExpression);
//
//            if (type == Type::Undefined())
//            {
//                type = expression->type();
//            }
//            else if (type != expression->type())
//            {
//                TODO("error type mismatch!!");
//            }
//        }
//
//        if (type == Type::Undefined())
//        {
//            // TODO Maybe we want to infer the types in the constructor in the future?
//            TODO("error missing type for field!!");
//        }
//
//        typeFields.append(new TypedFieldDefinitionNode(name, type, expression));
//        typeDefinition.addField(type, name, expression);
//    }
//
//    return typeFields;
//}
//
//QList<TypedMethodDefinitionStatement*> TypeChecker::typeCheckTypeMethodDefinitions(Type newRefType, Type newType, BlockNode* body)
//{
//    QList<TypedMethodDefinitionStatement*> methods;
//    for (const auto statement : body->statements())
//    {
//        if (statement->kind() != NodeKind::MethodDefinitionStatement)
//            continue;
//
//        methods.append(typeCheckTypeMethodDefinitionStatement(newRefType, newType, (MethodDefinitionStatement*)statement));
//    }
//    return methods;
//}
//
//QList<Parameter*> TypeChecker::typeCheckFunctionParameters(ParametersNode* parametersNode)
//{
//    QList<Parameter*> parameters;
//
//    for (const auto parameterNode : parametersNode->parameters())
//    {
//        auto parameterName = m_parseTree.tokens().getLexeme(parameterNode->name()->identifier());
//        auto parameterType = convertTypeNameToType(parameterNode->type());
//        currentScope()->addVariableBinding(parameterName, parameterType);
//
//        parameters.append(new Parameter(parameterName, parameterNode, parameterType));
//    }
//    return parameters;
//}
//
//TypedExpression* TypeChecker::typeCheckMemberAccessExpression(MemberAccessExpression* expression)
//{
//    auto thisType = currentScope()->tryGetTypeBinding(QStringView(u"this"));
//    auto& typeDefinition = m_typeDatabase.getTypeDefinition(thisType);
//    auto innerExpression = expression->expression();
//
//    switch (innerExpression->kind())
//    {
//        // Field access
//        case NodeKind::NameExpression:
//        {
//            auto nameExpression = (NameExpression*)innerExpression;
//            auto& identifier = nameExpression->identifier();
//            auto name = m_parseTree.tokens().getLexeme(identifier);
//            auto field = typeDefinition.getFieldByName(name);
//
//            if (field == nullptr)
//            {
//                // TODO print diagnostic if the field wasnt defined before
//                return nullptr;
//            }
//
//            return new TypedFieldAccessExpression(thisType, field, expression);
//        }
//        case NodeKind::FunctionCallExpression:
//        {
//            auto functionCallExpression = (FunctionCallExpression*)innerExpression;
//            auto& identifier = functionCallExpression->name();
//            auto name = m_parseTree.tokens().getLexeme(identifier);
//            auto& functionDefinition = typeDefinition.getFunctionDefinitionByName(name);
//            auto functionType = functionDefinition.type();
//            auto arguments = typeCheckFunctionCallArguments(functionCallExpression->arguments());
//
//            auto returnType = functionDefinition.returnType();
//            return new TypedMethodCallExpression(name, thisType, functionType, arguments, functionCallExpression, returnType);
//        }
//        default:
//        {
//            TODO("Missing MemberAccessExpression kind");
//            return nullptr;
//        }
//    }
//}
//
//TypedExpression* TypeChecker::typeCheckDiscardLiteral(DiscardLiteral* literal)
//{
//    return new Discard(literal);
//}
//
//TypedExpression* TypeChecker::typeCheckBoolLiteral(BoolLiteral* literal)
//{
//    return new BoolValue(literal);
//}

//QList<TypedExpression*> TypeChecker::typeCheckFunctionCallArguments(ArgumentsNode* argumentsNode)
//{
//    QList<TypedExpression*> arguments;
//
//    for (const auto argument : argumentsNode->arguments())
//    {
//        arguments.append(typeCheckExpression(argument));
//    }
//
//    return arguments;
//}

//Type TypeChecker::convertTypeNameToType(const TypeName& typeName)
//{
//    auto& nameToken = typeName.name()->identifier();
//    auto nameLexeme = m_parseTree.tokens().getLexeme(nameToken);
//
//    auto ref = (typeName.isReference() ? QString("ref ") : QString());
//    auto name = ref + nameLexeme.toString();
//
//    return m_typeDatabase.getTypeByName(name);
//}
//
//std::tuple<TypedExpression*, i32> TypeChecker::convertValueToTypedLiteral(QStringView valueLexeme, Type type, Node* source)
//{
//    if (type == Type::U8())
//    {
//        bool ok;
//        auto value = valueLexeme.toInt(&ok);
//        assert(ok);
//
//        // TODO add error for values outside of the u8 range
//        assert(value >= 0);
//        assert(value <= UINT8_MAX);
//
//        return { new U8Value((u8)value, source, type), value };
//    }
//    else if (type == Type::I32())
//    {
//        bool ok;
//        auto value = valueLexeme.toInt(&ok);
//        assert(ok);
//
//        // TODO add error for values outside of the i32 range
//
//        return { new I32Value(value, source, type), value };
//    }
//
//    return { nullptr, 0 };
//}
//
//std::tuple<TypedExpression*, i32> TypeChecker::convertValueToTypedLiteral(i32 value, Type type, Node* source)
//{
//    if (type == Type::U8())
//    {
//        // TODO add error for values outside of the u8 range
//        assert(value >= 0);
//        assert(value <= UINT8_MAX);
//
//        return { new U8Value((u8)value, source, type), value };
//    }
//    else if (type == Type::I32())
//    {
//        return { new I32Value(value, source, type), value };
//    }
//
//    return { nullptr, 0 };
//}
//