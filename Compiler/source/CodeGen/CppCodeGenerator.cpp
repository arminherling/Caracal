#include <CodeGen/CppCodeGenerator.h>

namespace Caracal
{
    static std::string ReplaceEscapeSequences(std::string_view input)
    {
        std::string result(input);

        auto replaceAll = [](std::string& str, const std::string& from, const std::string& to) {
            size_t startPos = 0;
            while ((startPos = str.find(from, startPos)) != std::string::npos)
            {
                str.replace(startPos, from.length(), to);
                startPos += to.length(); // Weiter nach dem ersetzten Teil suchen
            }
            };

        replaceAll(result, "\\\'", "\'");
        replaceAll(result, "\\\"", "\"");
        replaceAll(result, "\\a", "\a");
        replaceAll(result, "\\b", "\b");
        replaceAll(result, "\\f", "\f");
        replaceAll(result, "\\n", "\n");
        replaceAll(result, "\\r", "\r");
        replaceAll(result, "\\t", "\t");
        replaceAll(result, "\\v", "\v");
        replaceAll(result, "\\\\", "\\");

        return result;
    }

    [[nodiscard]] static auto InitializeTypeToCppName() noexcept
    {
        return std::unordered_map<Type, std::string_view>{
            { Type::Bool(), std::string_view("bool") },
            { Type::I32(), std::string_view("int") },
            { Type::F32(), std::string_view("float") },
            { Type::String(), std::string_view("std::string") },
        };
    }

    [[nodiscard]] static auto InitializeTypeToCppInclude() noexcept
    {
        return std::unordered_map<Type, std::string>{
            { Type::String(), std::string("#include <string>") },
        };
    }

    CppCodeGenerator::CppTypeDef* CppCodeGenerator::buildCppTypeDefinition(TypeDefinitionStatement* node) noexcept
    {
        const auto& nameExpression = node->nameExpression();
        const auto typeName = m_parseTree.tokens().getLexeme(nameExpression->nameToken());

        auto cppTypeDef = std::make_unique<CppTypeDef>();
        cppTypeDef->name = typeName;

        if(node->constructorParameters().has_value())
        {
            cppTypeDef->constructorParameters = node->constructorParameters().value().get();
        }

        // collect fields and methods
        const auto& statements = node->bodyNode()->statements();
        for (const auto& statement : statements)
        {
            const auto kind = statement->kind();
            if (kind == NodeKind::TypeFieldDeclaration)
            {
                const auto typeFieldDeclaration = (TypeFieldDeclaration*)statement.get();
                cppTypeDef->publicFields.emplace_back(typeFieldDeclaration);
            }
            else if (kind == NodeKind::MethodDefinitionStatement)
            {
                const auto methodDefinition = (MethodDefinitionStatement*)statement.get();
                const auto methodModifier = methodDefinition->modifier();
                const auto specialFunctionType = methodDefinition->specialFunctionType();

                if (methodModifier == MethodModifier::Public)
                {
                    cppTypeDef->publicMethods.emplace_back(methodDefinition);
                }
                else if (methodModifier == MethodModifier::Private)
                {
                    cppTypeDef->privateMethods.emplace_back(methodDefinition);
                }
                else if (methodModifier == MethodModifier::Static)
                {
                    cppTypeDef->staticMethods.emplace_back(methodDefinition);
                }
            }
        }

        m_cppTypeDefs.insert({ typeName, std::move(cppTypeDef) });
        return m_cppTypeDefs[typeName].get();
    }

    [[nodiscard]] std::string_view CppCodeGenerator::getCppNameForType(TypeNameNode* typeName) noexcept
    {
        static const auto cppTypeNames = InitializeTypeToCppName();
        if (const auto result = cppTypeNames.find(typeName->type()); result != cppTypeNames.end())
            return result->second;

        return m_parseTree.tokens().getLexeme(typeName->name()->nameToken());
    }

    [[nodiscard]] static std::string_view GetCppNameForType(Type type) noexcept
    {
        static const auto cppTypeNames = InitializeTypeToCppName();
        if (const auto result = cppTypeNames.find(type); result != cppTypeNames.end())
            return result->second;

        return std::string_view();
    }

    [[nodiscard]] static std::optional<std::string> GetCppIncludeForType(Type type) noexcept
    {
        static const auto cppIncludes = InitializeTypeToCppInclude();
        if (const auto result = cppIncludes.find(type); result != cppIncludes.end())
            return result->second;

        return std::nullopt;
    }

    [[nodiscard]] static std::string StringifyBinaryOperator(BinaryOperatorKind kind)
    {
        switch (kind)
        {
            case BinaryOperatorKind::Addition:
                return std::string("+");
            case BinaryOperatorKind::Subtraction:
                return std::string("-");
            case BinaryOperatorKind::Multiplication:
                return std::string("*");
            case BinaryOperatorKind::Division:
                return std::string("/");
            case BinaryOperatorKind::Equal:
                return std::string("==");
            case BinaryOperatorKind::NotEqual:
                return std::string("!=");
            case BinaryOperatorKind::LessThan:
                return std::string("<");
            case BinaryOperatorKind::LessOrEqual:
                return std::string("<=");
            case BinaryOperatorKind::GreaterThan:
                return std::string(">");
            case BinaryOperatorKind::GreaterOrEqual:
                return std::string(">=");
            case BinaryOperatorKind::LogicalAnd:
                return std::string("&&");
            case BinaryOperatorKind::LogicalOr:
                return std::string("||");
            default:
                TODO("Implement StringifyBinaryOperator for all operators");
                return std::string();
        }
    }

    [[nodiscard]] static std::string StringifyUnaryOperator(UnaryOperatorKind kind)
    {
        switch (kind)
        {
            case UnaryOperatorKind::LogicalNegation:
                return std::string("!");
            case UnaryOperatorKind::ValueNegation:
                return std::string("-");
            case UnaryOperatorKind::ReferenceOf:
                return std::string("&");
            default:
                TODO("Implement StringifyUnaryOperator for all operators");
                return std::string();
        }
    }

    CppCodeGenerator::CppCodeGenerator(const ParseTree& parseTree, i32 indentation)
        : m_parseTree{ parseTree }
        , m_currentScope{ Scope::Global }
        , m_discardCount{ 0 }
        , m_currentStatement{ NodeKind::Unknown }
        , m_cppIncludes{ indentation }
        , m_forwardDeclarations{ indentation }
        , m_builder{ indentation }
    {
    }

    std::string CppCodeGenerator::generate()
    {
        for (const auto& statement : m_parseTree.statements())
        {
            generateNode(statement.get());
        }

        if (!m_cppIncludes.isEmpty())
        {
            m_cppIncludes.appendLine("");
        }
        const auto includes = m_cppIncludes.toString();

        if (!m_forwardDeclarations.isEmpty())
        {
            m_forwardDeclarations.appendLine("");
        }
        const auto forwardDeclarations = m_forwardDeclarations.toString();

        // generate cpp method definitions
        for (const auto& [typeName, cppTypeDef] : m_cppTypeDefs)
        {
            generateConstructorDefinition(cppTypeDef.get());

            for (const auto& staticMethod : cppTypeDef->staticMethods)
            {
                generateMethodDefinition(typeName, staticMethod);
            }
            for (const auto& publicMethod : cppTypeDef->publicMethods)
            {
                generateMethodDefinition(typeName, publicMethod);
            }
            for (const auto& privateMethod : cppTypeDef->privateMethods)
            {
                generateMethodDefinition(typeName, privateMethod);
            }
        }

        return includes + forwardDeclarations + m_builder.toString();
    }

    void CppCodeGenerator::generateNode(Node* node) noexcept
    {
        switch (node->kind())
        {
            case NodeKind::ConstantDeclaration:
            {
                const auto declaration = (ConstantDeclaration*)node;
                const auto isDiscard = declaration->leftExpression()->kind() == NodeKind::DiscardLiteral;
                if (isDiscard)
                {
                    if (m_currentScope == Scope::Global)
                    {
                        generateGlobalDiscardedExpression(declaration->rightExpression().get());
                    }
                    else
                    {
                        generateLocalDiscardedExpression(declaration->rightExpression().get());
                    }
                }
                else
                {
                    generateConstantDeclaration((ConstantDeclaration*)node);
                }
                break;
            }
            case NodeKind::VariableDeclaration:
            {
                const auto declaration = (VariableDeclaration*)node;
                const auto isDiscard = declaration->leftExpression()->kind() == NodeKind::DiscardLiteral;
                if (isDiscard)
                {
                    if (!declaration->rightExpression().has_value())
                    {
                        TODO("Variable declaration with discard literal and no right expression shouldnt get here");
                    }
                    generateLocalDiscardedExpression(declaration->rightExpression().value().get());
                    return;
                }
                else
                {
                    generateVariableDeclaration((VariableDeclaration*)node);
                    break;
                }
            }
            case NodeKind::CppBlockStatement:
            {
                m_currentStatement = NodeKind::CppBlockStatement;
                generateCppBlock((CppBlockStatement*)node);
                break;
            }
            case NodeKind::ExpressionStatement:
            {
                m_currentStatement = NodeKind::ExpressionStatement;
                generateExpressionStatement((ExpressionStatement*)node);
                break;
            }
            case NodeKind::AssignmentStatement:
            {
                m_currentStatement = NodeKind::AssignmentStatement;
                generateAssignmentStatement((AssignmentStatement*)node);
                break;
            }
            case NodeKind::TypeDefinitionStatement:
            {
                m_currentStatement = NodeKind::TypeDefinitionStatement;
                generateTypeDefinitionStatement((TypeDefinitionStatement*)node);
                break;
            }
            case NodeKind::FunctionDefinitionStatement:
            {
                m_currentStatement = NodeKind::FunctionDefinitionStatement;
                generateFunctionDefinition((FunctionDefinitionStatement*)node);
                break;
            }
            case NodeKind::EnumDefinitionStatement:
            {
                m_currentStatement = NodeKind::EnumDefinitionStatement;
                generateEnumDefinitionStatement((EnumDefinitionStatement*)node);
                break;
            }
            case NodeKind::IfStatement:
            {
                m_currentStatement = NodeKind::IfStatement;
                generateIfStatement((IfStatement*)node);
                break;
            }
            case NodeKind::WhileStatement:
            {
                m_currentStatement = NodeKind::WhileStatement;
                generateWhileStatement((WhileStatement*)node);
                break;
            }
            case NodeKind::BreakStatement:
            {
                m_currentStatement = NodeKind::BreakStatement;
                generateBreakStatement((BreakStatement*)node);
                break;
            }
            case NodeKind::SkipStatement:
            {
                m_currentStatement = NodeKind::SkipStatement;
                generateSkipStatement((SkipStatement*)node);
                break;
            }
            case NodeKind::ReturnStatement:
            {
                m_currentStatement = NodeKind::ReturnStatement;
                generateReturnStatement((ReturnStatement*)node);
                break;
            }
            case NodeKind::GroupingExpression:
            {
                generateGroupingExpression((GroupingExpression*)node);
                break;
            }
            case NodeKind::UnaryExpression:
            {
                generateUnaryExpression((UnaryExpression*)node);
                break;
            }
            case NodeKind::BinaryExpression:
            {
                generateBinaryExpression((BinaryExpression*)node);
                break;
            }
            case NodeKind::NameExpression:
            {
                generateNameExpression((NameExpression*)node);
                break;
            }
            case NodeKind::FunctionCallExpression:
            {
                auto functionCallExpression = (FunctionCallExpression*)node;
                // if name is print use builtin
                auto& nameToken = functionCallExpression->nameExpression()->nameToken();
                if (m_parseTree.tokens().getLexeme(nameToken) == std::string_view("print"))
                {
                    generateBuiltinPrintFunction(functionCallExpression->argumentsNode().get());
                    break;
                }

                generateFunctionCallExpression(functionCallExpression);
                break;
            }
            case NodeKind::MemberAccessExpression:
            {
                generateMemberAccessExpression((MemberAccessExpression*)node);
                break;
            }
            case NodeKind::BoolLiteral:
            {
                generateBoolLiteral((BoolLiteral*)node);
                break;
            }
            case NodeKind::NumberLiteral:
            {
                generateNumberLiteral((NumberLiteral*)node);
                break;
            }
            case NodeKind::StringLiteral:
            {
                generateStringLiteral((StringLiteral*)node);
                break;
            }
            case NodeKind::BlockNode:
            {
                m_currentStatement = NodeKind::BlockNode;
                generateBlockNode((BlockNode*)node);
                break;
            }

            default:
            {
                m_builder.appendIndentedLine("Missing NodeKind!!");
                break;
            }
        }
    }

    void CppCodeGenerator::generateCppBlock(CppBlockStatement* node) noexcept
    {
        const auto& lines = node->lines();
        for (const auto& line : lines)
        {
            auto lexeme = m_parseTree.tokens().getLexeme(line);
            // remove the first and last quotes
            lexeme = lexeme.substr(1, lexeme.length() - 2);
            const auto result = ReplaceEscapeSequences(lexeme);
            m_builder.appendIndentedLine(result);
        }
    }

    void CppCodeGenerator::generateBlockNode(BlockNode* node) noexcept
    {
        const auto& statements = node->statements();
        m_builder.appendIndentedLine("{");
        m_builder.pushIndentation();
        for (const auto& statement : statements)
        {
            generateNode(statement.get());
        }
        m_builder.popIndentation();
        m_builder.appendIndentedLine("}");
    }

    void CppCodeGenerator::generateExpressionStatement(ExpressionStatement* node) noexcept
    {
        m_builder.appendIndented("");
        generateNode(node->expression().get());
        m_builder.appendLine(";");
    }

    void CppCodeGenerator::generateConstantDeclaration(ConstantDeclaration* node) noexcept
    {
        if (node->explicitType().has_value())
        {
            const auto typeNameNode = node->explicitType().value().get();
            const auto typeName = getCppNameForType(typeNameNode);
            const auto include = GetCppIncludeForType(typeNameNode->type());
            if (include.has_value())
            {
                m_cppIncludes.appendLine(include.value());
            }
            m_builder.appendIndented("const ").append(typeName);
        }
        else
        {
            const auto type = node->type();
            const auto cppTypeName = GetCppNameForType(type);
            const auto include = GetCppIncludeForType(type);
            if (include.has_value())
            {
                m_cppIncludes.appendLine(include.value());
            }

            if (cppTypeName.empty())
            {
                m_builder.appendIndented("const auto");
            }
            else
            {
                m_builder.appendIndented("const ").append(cppTypeName);
            }
        }

        // check if right expression is a ref
        if (node->rightExpression()->kind() == NodeKind::UnaryExpression)
        {
            const auto unaryExpression = (UnaryExpression*)node->rightExpression().get();
            if (unaryExpression->unaryOperator() == UnaryOperatorKind::ReferenceOf)
            {
                m_builder.append(StringifyUnaryOperator(UnaryOperatorKind::ReferenceOf));
            }
        }
        m_builder.append(" ");

        generateNode(node->leftExpression().get());
        m_builder.append(" = ");
        generateNode(node->rightExpression().get());
        m_builder.appendLine(";");
    }

    void CppCodeGenerator::generateVariableDeclaration(VariableDeclaration* node) noexcept
    {
        if (node->explicitType().has_value())
        {
            const auto typeNameNode = node->explicitType().value().get();
            const auto typeName = getCppNameForType(typeNameNode);
            const auto include = GetCppIncludeForType(typeNameNode->type());
            if (include.has_value())
            {
                m_cppIncludes.appendLine(include.value());
            }
            m_builder.appendIndented(typeName);
        }
        else
        {
            const auto type = node->type();
            const auto include = GetCppIncludeForType(type);
            if (include.has_value())
            {
                m_cppIncludes.appendLine(include.value());
            }
            m_builder.appendIndented("auto");
        }

        // check if right expression is a ref
        if (node->rightExpression().has_value() && node->rightExpression().value()->kind() == NodeKind::UnaryExpression)
        {
            const auto unaryExpression = (UnaryExpression*)node->rightExpression().value().get();
            if (unaryExpression->unaryOperator() == UnaryOperatorKind::ReferenceOf)
            {
                m_builder.append(StringifyUnaryOperator(UnaryOperatorKind::ReferenceOf));
            }
        }
        m_builder.append(" ");

        generateNode(node->leftExpression().get());
        if (node->rightExpression().has_value())
        {
            m_builder.append(" = ");
            generateNode(node->rightExpression().value().get());
        }
        m_builder.appendLine(";");
    }

    void CppCodeGenerator::generateTypeFieldDeclaration(TypeFieldDeclaration* node) noexcept
    {
        m_builder.appendIndented("");
        if (node->isConstant())
        {
            m_builder.append("const ");
        }

        if (node->explicitType().has_value())
        {
            const auto typeNameNode = node->explicitType().value().get();
            const auto typeName = getCppNameForType(typeNameNode);
            const auto include = GetCppIncludeForType(typeNameNode->type());
            if (include.has_value())
            {
                m_cppIncludes.appendLine(include.value());
            }
            m_builder.append(typeName).append(" ");
        }
        else
        {
            const auto type = node->type();
            const auto typeName = GetCppNameForType(node->type());
            const auto include = GetCppIncludeForType(type);
            if (include.has_value())
            {
                m_cppIncludes.appendLine(include.value());
            }
            m_builder.append(typeName).append(" ");
        }

        m_builder.append(generateTypeFieldName(node->nameExpression().get()));

        if (node->rightExpression().has_value())
        {
            if (node->rightExpression().value()->isLiteral())
            {
                m_builder.append(" = ");
                generateNode(node->rightExpression().value().get());
            }
        }
        m_builder.appendLine(";");
    }

    void CppCodeGenerator::generateConstructorDeclarationSignature(std::string_view className, ParametersNode* parametersNode) noexcept
    {
        if(parametersNode == nullptr)
        {
            m_builder.appendIndented(className).appendLine("() = default;");
            return;
        }

        const auto isEmpty = parametersNode->parameters().empty();

        m_builder.appendIndented(className).append(generateFunctionSignatureParameterPart(parametersNode));

        if (isEmpty)
        {
            m_builder.append(" = default");
        }

        m_builder.appendLine(";");
    }

    void CppCodeGenerator::generateConstructorDefinition(CppTypeDef* cppType) noexcept
    {
        const auto& typeName = cppType->name;
        const auto& parametersNode = cppType->constructorParameters;
        if (parametersNode == nullptr)
            return;

        const auto parameterPart = generateFunctionSignatureParameterPart(parametersNode);

        m_builder.append(typeName).append("::").append(typeName).appendLine(parameterPart);
        
        m_builder.pushIndentation();

        StringBuilder initList;
        bool initListIsEmpty = true;
        for (const auto& field : cppType->publicFields)
        {
            if (field->rightExpression().has_value())
            {
                if (field->rightExpression().value()->isLiteral())
                {
                    continue;
                }

                const auto expression = field->rightExpression().value().get();
                if (expression->kind() != NodeKind::NameExpression)
                {
                    TODO("Only NameExpressions are supported in constructor initializers for now");
                }
                
                const auto nameExpression = (NameExpression*)expression;
                const auto fieldName = generateTypeFieldName(field->nameExpression().get());
                const auto parameterName = m_parseTree.tokens().getLexeme(nameExpression->nameToken());

                if(!initListIsEmpty)
                {
                    initList.append(", ");
                }
                initList.append(fieldName).append("{ ").append(parameterName).append(" }");
                initListIsEmpty = false;
            }
        }

        if(!initListIsEmpty)
        {
            m_builder.appendIndented(": ").appendLine(initList.toString());
        }

        m_builder.popIndentation();
        m_builder.appendLine("{").appendLine("}").appendLine("");
    }

    void CppCodeGenerator::generateDestructorDeclarationSignature(std::string_view className) noexcept
    {
        m_builder.appendIndented("~").append(className).appendLine("() = default;");
    }

    void CppCodeGenerator::generateMethodDeclarationSignature(MethodDefinitionStatement* node) noexcept
    {
        auto nameExpression = node->methodNameNode()->methodNameExpression().get();
        const auto methodName = m_parseTree.tokens().getLexeme(nameExpression->nameToken());
        auto parametersNode = node->parametersNode().get();
        auto returnTypesNode = node->returnTypesNode().get();
        const auto specialFunctionType = node->specialFunctionType();

        m_builder.appendIndented("");
        if (node->modifier() == MethodModifier::Static)
        {
            m_builder.append("static ");
        }

        m_builder.append(generateFunctionSignatureReturnPart(returnTypesNode, false));
        m_builder.append(generateFunctionSignatureNamePart(methodName));
        m_builder.append(generateFunctionSignatureParameterPart(parametersNode));

        if (specialFunctionType != SpecialFunctionType::None)
        {
            const auto& statements = node->bodyNode()->statements();
            if (statements.empty())
            {
                m_builder.append(" = default");
            }
        }
        
        m_builder.appendLine(";");
    }

    void CppCodeGenerator::generateMethodDefinition(const std::string_view& typeName, MethodDefinitionStatement* node) noexcept
    {
        auto nameExpression = node->methodNameNode()->methodNameExpression().get();
        const auto functionName = m_parseTree.tokens().getLexeme(nameExpression->nameToken());
        const auto isMainFunction = functionName == std::string_view("main");
        auto parametersNode = node->parametersNode().get();
        auto returnTypesNode = node->returnTypesNode().get();
        
        const auto returnPart = generateFunctionSignatureReturnPart(returnTypesNode, isMainFunction);
        const auto namePart = generateFunctionSignatureNamePart(functionName);
        const auto parameterPart = generateFunctionSignatureParameterPart(parametersNode);
        const auto signature = returnPart + std::string(typeName) + "::" + namePart + parameterPart;

        m_builder.appendLine(signature).appendLine("{");
        m_builder.pushIndentation();
        const auto& body = node->bodyNode();
        for (const auto& statement : body->statements())
        {
            generateNode(statement.get());
        }
        m_builder.popIndentation();
        m_builder.appendLine("}").appendLine("");
    }

    void CppCodeGenerator::generateGlobalDiscardedExpression(Expression* expression) noexcept
    {
        m_builder.appendIndented("const auto _").append(std::to_string(m_discardCount++)).append(" = ");
        generateNode(expression);
        m_builder.appendLine(";");
    }

    void CppCodeGenerator::generateLocalDiscardedExpression(Expression* expression) noexcept
    {
        m_builder.appendIndented("static_cast<void>(");
        generateNode(expression);
        m_builder.appendLine(");");
    }

    void CppCodeGenerator::generateAssignmentStatement(AssignmentStatement* node) noexcept
    {
        m_builder.appendIndented("");
        generateNode(node->leftExpression().get());
        m_builder.append(" = ");
        generateNode(node->rightExpression().get());
        m_builder.appendLine(";");
    }

    std::string CppCodeGenerator::generateEnumSignature(EnumDefinitionStatement* node) noexcept
    {
        const auto enumName = m_parseTree.tokens().getLexeme(node->nameExpression()->nameToken());

        StringBuilder signature;
        signature.appendIndented("enum class ").append(enumName);
        if (node->baseType().has_value())
        {
            const auto baseType = node->baseType().value().get();
            const auto cppTypeName = GetCppNameForType(baseType->type());

            signature.append(" : ").append(cppTypeName);
        }
        else
        {
            signature.append(" : unsigned char"); // 1 byte default size if not specified
        }

        return signature.toString();
    }

    std::string CppCodeGenerator::generateFunctionSignatureReturnPart(ReturnTypesNode* returnTypesNode, bool isMainFunction) noexcept
    {
        const auto& returnTypes = returnTypesNode->returnTypes();
        const auto hasReturnTypes = !returnTypes.empty();

        StringBuilder signature;

        if (!hasReturnTypes)
        {
            if (isMainFunction)
            {
                signature.append("int ");
            }
            else
            {
                signature.append("void ");
            }
        }
        else
        {
            if (returnTypes.size() != 1)
            {
                TODO("Implement multiple return types in CppCodeGenerator::generateFunctionSignature");
            }

            const auto returnType = returnTypes[0]->type();
            const auto include = GetCppIncludeForType(returnType);
            if (include.has_value())
            {
                m_cppIncludes.appendLine(include.value());
            }
            const auto returnTypeName = GetCppNameForType(returnType);
            signature.append(returnTypeName).append(" ");
        }

        return signature.toString();
    }

    std::string CppCodeGenerator::generateFunctionSignatureNamePart(std::string_view functionName) noexcept
    {
        if (functionName.starts_with('_'))
        {
            // remove first letter
            return std::string(functionName.substr(1));
        }
        else
        {
            return std::string(functionName);
        }
    }

    std::string CppCodeGenerator::generateFunctionSignatureParameterPart(ParametersNode* parametersNode) noexcept
    {
        const auto& parameters = parametersNode->parameters();
        StringBuilder signature;
        signature.append("(");
        for (const auto& parameter : parameters)
        {
            const auto typeName = getCppNameForType(parameter->typeName().get());
            if (parameter->typeName()->isReference())
            {
                signature.append(typeName).append("& ");
            }
            else
            {
                signature.append(typeName).append(" ");
            }
            const auto& parameterNameToken = parameter->nameExpression()->nameToken();
            signature.append(std::string(m_parseTree.tokens().getLexeme(parameterNameToken)));

            if (&parameter != &parameters.back())
            {
                signature.append(", ");
            }
        }
        signature.append(")");

        return signature.toString();
    }

    void CppCodeGenerator::generateTypeDefinitionStatement(TypeDefinitionStatement* node) noexcept
    {
        auto cppTypeDef = buildCppTypeDefinition(node);
        auto typeName = cppTypeDef->name;

        m_forwardDeclarations.append("class ").append(typeName).appendLine(";");

        m_builder.append("class ").appendLine(typeName);
        m_builder.appendIndentedLine("{");
        m_builder.pushIndentation();
        {
            const auto hasPublicMethods = (!cppTypeDef->publicMethods.empty() || !cppTypeDef->staticMethods.empty());
            const auto hasPrivateMethods = (!cppTypeDef->privateMethods.empty());
            const auto hasPublicFields = (!cppTypeDef->publicFields.empty());

            m_builder.appendLine("public:");

            generateConstructorDeclarationSignature(typeName, cppTypeDef->constructorParameters);
            generateDestructorDeclarationSignature(typeName);

            if (hasPublicMethods)
            {
                m_builder.appendLine("");
            }

            for (const auto& staticMethod : cppTypeDef->staticMethods)
            {
                generateMethodDeclarationSignature(staticMethod);
            }
            for (const auto& publicMethod : cppTypeDef->publicMethods)
            {
                generateMethodDeclarationSignature(publicMethod);
            }

            if (hasPrivateMethods)
            {
                m_builder.appendLine("").appendLine("private:");
                for (const auto& privateMethod : cppTypeDef->privateMethods)
                {
                    generateMethodDeclarationSignature(privateMethod);
                }
            }

            if (hasPublicFields)
            {
                if (hasPrivateMethods)
                {
                    m_builder.appendLine("");
                }

                m_builder.appendLine("").appendLine("public:");
                for (const auto& publicField : cppTypeDef->publicFields)
                {
                    generateTypeFieldDeclaration(publicField);
                }
            }
        }
        m_builder.popIndentation();
        m_builder.appendIndentedLine("};").appendLine("");
    }

    std::string CppCodeGenerator::generateTypeFieldName(NameExpression* node) noexcept
    {
        const auto fieldName = m_parseTree.tokens().getLexeme(node->nameToken());
        if (fieldName.starts_with('_'))
        {
            return "m" + std::string(fieldName);
        }
        else
        {
            return "m_" + std::string(fieldName);
        }
    }

    void CppCodeGenerator::generateEnumDefinitionStatement(EnumDefinitionStatement* node) noexcept
    {
        const auto signature = generateEnumSignature(node);
        m_forwardDeclarations.append(signature).appendLine(";");
        m_builder.appendIndentedLine(signature);
        m_builder.appendIndentedLine("{");
        m_builder.pushIndentation();

        for (const auto& fieldNode : node->fieldNodes())
        {
            m_builder.appendIndented("");
            generateNameExpression(fieldNode->nameExpression().get());
            if (fieldNode->valueExpression().has_value())
            {
                m_builder.append(" = ");
                generateNode(fieldNode->valueExpression().value().get());
            }
            m_builder.appendLine(",");
        }
        
        m_builder.popIndentation();
        m_builder.appendIndentedLine("};").appendLine("");
    }

    void CppCodeGenerator::generateFunctionDefinition(FunctionDefinitionStatement* node) noexcept
    {
        const auto oldScope = m_currentScope;
        m_currentScope = Scope::Function;

        auto nameExpression = node->nameExpression().get();
        const auto functionName = m_parseTree.tokens().getLexeme(nameExpression->nameToken());
        const auto isMainFunction = functionName == std::string_view("main");
        auto parametersNode = node->parametersNode().get();
        auto returnTypesNode = node->returnTypesNode().get();

        const auto returnPart = generateFunctionSignatureReturnPart(returnTypesNode, isMainFunction);
        const auto namePart = generateFunctionSignatureNamePart(functionName);
        const auto parameterPart = generateFunctionSignatureParameterPart(parametersNode);
        const auto signature = returnPart + std::string(namePart) + parameterPart;

        m_forwardDeclarations.append(signature).appendLine(";");
        m_builder.appendIndentedLine(signature).appendIndentedLine("{");

        m_builder.pushIndentation();
        const auto& body = node->bodyNode();
        for (const auto& statement : body->statements())
        {
            generateNode(statement.get());
        }
        m_builder.popIndentation();

        m_builder.appendIndentedLine("}").appendLine("");

        m_currentScope = oldScope; // Reset the scope after generating the function definition
    }

    void CppCodeGenerator::generateIfStatement(IfStatement* node) noexcept
    {
        const auto& condition = node->condition();
        const auto needsParentheses = condition->kind() != NodeKind::GroupingExpression;

        // unwrap the condition if it is a grouping expression
        if (needsParentheses)
        {
            m_builder.appendIndented("if (");
        }
        else
        {
            m_builder.appendIndented("if ");
        }
        generateNode(node->condition().get());
        if (needsParentheses)
        {
            m_builder.appendLine(")");
        }
        else
        {
            m_builder.appendLine("");
        }

        const auto trueBlockNeedsBrackets = node->trueStatement()->kind() != NodeKind::BlockNode;
        if (trueBlockNeedsBrackets)
        {
            m_builder.appendIndentedLine("{");
            m_builder.pushIndentation();
        }
        generateNode(node->trueStatement().get());
        if (trueBlockNeedsBrackets)
        {
            m_builder.popIndentation();
            m_builder.appendIndentedLine("}");
        }

        // Check if there is an else block
        if (node->hasFalseBlock())
        {
            const auto& falseStatement = node->falseStatement().value();
            const auto falseBlockNeedsBrackets = falseStatement->kind() != NodeKind::BlockNode;

            m_builder.appendIndentedLine("else");
            if (falseBlockNeedsBrackets)
            {
                m_builder.appendIndentedLine("{");
                m_builder.pushIndentation();
            }
            generateNode(falseStatement.get());
            if (falseBlockNeedsBrackets)
            {
                m_builder.popIndentation();
                m_builder.appendIndentedLine("}");
            }
        }
    }

    void CppCodeGenerator::generateWhileStatement(WhileStatement* node) noexcept
    {
        const auto& condition = node->condition();
        const auto needsParentheses = condition->kind() != NodeKind::GroupingExpression;

        // unwrap the condition if it is a grouping expression
        if (needsParentheses)
        {
            m_builder.appendIndented("while (");
        }
        else
        {
            m_builder.appendIndented("while ");
        }
        generateNode(node->condition().get());
        if (needsParentheses)
        {
            m_builder.appendLine(")");
        }
        else
        {
            m_builder.appendLine("");
        }

        const auto bodyNeedsBrackets = node->trueStatement()->kind() != NodeKind::BlockNode;
        if (bodyNeedsBrackets)
        {
            m_builder.appendIndentedLine("{");
            m_builder.pushIndentation();
        }
        generateNode(node->trueStatement().get());
        if (bodyNeedsBrackets)
        {
            m_builder.popIndentation();
            m_builder.appendIndentedLine("}");
        }
    }

    void CppCodeGenerator::generateBreakStatement(BreakStatement* /*node*/) noexcept
    {
        m_builder.appendIndentedLine("break;");
    }

    void CppCodeGenerator::generateSkipStatement(SkipStatement* /*node*/) noexcept
    {
        m_builder.appendIndentedLine("continue;");
    }

    void CppCodeGenerator::generateReturnStatement(ReturnStatement* node) noexcept
    {
        m_builder.appendIndented("return");
        if (node->expression().has_value())
        {
            m_builder.append(" ");
            generateNode(node->expression().value().get());
        }
        m_builder.appendLine(";");
    }

    void CppCodeGenerator::generateGroupingExpression(GroupingExpression* node) noexcept
    {
        m_builder.append("(");
        generateNode(node->expression().get());
        m_builder.append(")");
    }

    void CppCodeGenerator::generateUnaryExpression(UnaryExpression* node) noexcept
    {
        if (node->unaryOperator() != UnaryOperatorKind::ReferenceOf)
        {
            const auto unaryOperator = StringifyUnaryOperator(node->unaryOperator());
            m_builder.append(unaryOperator);
        }
        generateNode(node->expression().get());
    }

    void CppCodeGenerator::generateBinaryExpression(BinaryExpression* node) noexcept
    {
        const auto binaryOperator = node->binaryOperator();
        if (binaryOperator == BinaryOperatorKind::MemberAccess)
        {
            generateNode(node->leftExpression().get());
            m_builder.append("::");
            generateNode(node->rightExpression().get());
        }
        else
        {
            const auto binaryOperatorString = StringifyBinaryOperator(node->binaryOperator());

            generateNode(node->leftExpression().get());
            m_builder.append(" ").append(binaryOperatorString).append(" ");
            generateNode(node->rightExpression().get());
        }
    }

    void CppCodeGenerator::generateNameExpression(NameExpression* node) noexcept
    {
        m_builder.append(m_parseTree.tokens().getLexeme(node->nameToken()));
    }

    void CppCodeGenerator::generateFunctionCallExpression(FunctionCallExpression* node) noexcept
    {
        generateNode(node->nameExpression().get());
        m_builder.append("(");
        const auto& arguments = node->argumentsNode()->arguments();
        for (const auto& argument : arguments)
        {
            generateNode(argument.get());

            if (&argument != &arguments.back())
            {
                m_builder.append(", ");
            }
        }
        m_builder.append(")");
    }

    void CppCodeGenerator::generateMemberAccessExpression(MemberAccessExpression* node) noexcept
    {
        auto expression = node->expression().get();
        m_builder.append("this->");
        if (expression->kind() == NodeKind::FunctionCallExpression)
        {
            generateFunctionCallExpression((FunctionCallExpression*)expression);
        }
        else
        {
            m_builder.append(generateTypeFieldName((NameExpression*)expression));
        }
    }

    void CppCodeGenerator::generateBoolLiteral(BoolLiteral* node) noexcept
    {
        auto value = node->value() ? std::string_view("true") : std::string_view("false");
        m_builder.append(value);
    }

    void CppCodeGenerator::generateNumberLiteral(NumberLiteral* node) noexcept
    {
        auto lexeme = m_parseTree.tokens().getLexeme(node->literalToken());
        m_builder.append(lexeme);
    }

    void CppCodeGenerator::generateStringLiteral(StringLiteral* node) noexcept
    {
        auto lexeme = m_parseTree.tokens().getLexeme(node->literalToken());
        m_builder.append("std::string{").append(lexeme).append("}");
    }

    void CppCodeGenerator::generateBuiltinPrintFunction(ArgumentsNode* node) noexcept
    {
        m_cppIncludes.appendLine("#include <iostream>");

        if (m_currentStatement == NodeKind::ExpressionStatement)
        {
            m_builder.append("std::cout << ");
        }
        else
        {
            m_builder.append("&(std::cout << ");
        }

        const auto& arguments = node->arguments();

        for (const auto& argument : arguments)
        {
            const auto type = argument->type();
            const auto include = GetCppIncludeForType(type);
            if (include.has_value())
            {
                m_cppIncludes.appendLine(include.value());
            }

            generateNode(argument.get());

            if (&argument != &arguments.back())
            {
                m_builder.append(" << ");
            }
        }

        if (m_currentStatement == NodeKind::ExpressionStatement)
        {
            m_builder.append(" << \"\\n\"");
        }
        else
        {
            m_builder.append(" << \"\\n\")");
        }
    }

    std::string generateCpp(const ParseTree& parseTree) noexcept
    {
        CppCodeGenerator generator{ parseTree };
        return generator.generate();
    }
}
