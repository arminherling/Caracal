#include <CodeGen/CppCodeGenerator.h>
#include <QStringBuilder>

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
        return std::unordered_map<Type, QString>{
            { Type::String(), QStringLiteral("#include <string>") },
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

    [[nodiscard]] static std::optional<QString> GetCppIncludeForType(Type type) noexcept
    {
        static const auto cppIncludes = InitializeTypeToCppInclude();
        if (const auto result = cppIncludes.find(type); result != cppIncludes.end())
            return result->second;

        return std::nullopt;
    }

    [[nodiscard]] static QString StringifyBinaryOperator(BinaryOperatorKind kind)
    {
        switch (kind)
        {
            case BinaryOperatorKind::Addition:
                return QStringLiteral("+");
            case BinaryOperatorKind::Subtraction:
                return QStringLiteral("-");
            case BinaryOperatorKind::Multiplication:
                return QStringLiteral("*");
            case BinaryOperatorKind::Division:
                return QStringLiteral("/");
            case BinaryOperatorKind::Equal:
                return QStringLiteral("==");
            case BinaryOperatorKind::NotEqual:
                return QStringLiteral("!=");
            case BinaryOperatorKind::LessThan:
                return QStringLiteral("<");
            case BinaryOperatorKind::LessOrEqual:
                return QStringLiteral("<=");
            case BinaryOperatorKind::GreaterThan:
                return QStringLiteral(">");
            case BinaryOperatorKind::GreaterOrEqual:
                return QStringLiteral(">=");
            case BinaryOperatorKind::LogicalAnd:
                return QStringLiteral("&&");
            case BinaryOperatorKind::LogicalOr:
                return QStringLiteral("||");
            default:
                TODO("Implement StringifyBinaryOperator for all operators");
                return QString();
        }
    }

    [[nodiscard]] static QString StringifyUnaryOperator(UnaryOperatorKind kind)
    {
        switch (kind)
        {
            case UnaryOperatorKind::LogicalNegation:
                return QStringLiteral("!");
            case UnaryOperatorKind::ValueNegation:
                return QStringLiteral("-");
            case UnaryOperatorKind::ReferenceOf:
                return QStringLiteral("&");
            default:
                TODO("Implement StringifyUnaryOperator for all operators");
                return QString();
        }
    }

    CppCodeGenerator::CppCodeGenerator(ParseTree& parseTree, i32 indentation)
        : BasePrinter(indentation)
        , m_parseTree{ parseTree }
        , m_cppIncludes{ }
        , m_forwardDeclarations{ }
        , m_currentScope(Scope::Global)
        , m_discardCount(0)
        , m_currentStatement(NodeKind::Unknown)
    {
    }

    QString CppCodeGenerator::generate()
    {
        for (const auto& statement : m_parseTree.statements())
        {
            generateNode(statement.get());
        }

        if (!m_cppIncludes.isEmpty())
        {
            m_cppIncludes.append(newLine());
        }
        const auto includes = m_cppIncludes.join("");

        if (!m_forwardDeclarations.isEmpty())
        {
            m_forwardDeclarations.append(newLine());
        }
        const auto forwardDeclarations = m_forwardDeclarations.join("");

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

        return includes % forwardDeclarations % toUtf8();
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
                stream() << indentation() << QString("Missing NodeKind!!") << newLine();
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
            stream() << indentation() << QString::fromStdString(result) << newLine();
        }
    }

    void CppCodeGenerator::generateBlockNode(BlockNode* node) noexcept
    {
        const auto& statements = node->statements();
        stream() << indentation() << "{" << newLine();
        pushIndentation();
        for (const auto& statement : statements)
        {
            generateNode(statement.get());
        }
        popIndentation();
        stream() << indentation() << "}" << newLine();
    }

    void CppCodeGenerator::generateExpressionStatement(ExpressionStatement* node) noexcept
    {
        stream() << indentation();
        generateNode(node->expression().get());
        stream() << ";" << newLine();
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
                m_cppIncludes.append(include.value() % newLine());
            }
            stream() << indentation() << "const " << QString::fromStdString(std::string(typeName));
        }
        else
        {
            const auto type = node->type();
            const auto cppTypeName = GetCppNameForType(type);
            const auto include = GetCppIncludeForType(type);
            if (include.has_value())
            {
                m_cppIncludes.append(include.value() % newLine());
            }

            if (cppTypeName.empty())
            {
                stream() << indentation() << "const auto";
            }
            else
            {
                stream() << indentation() << "const " << QString::fromStdString(std::string(cppTypeName));
            }
        }

        // check if right expression is a ref
        if (node->rightExpression()->kind() == NodeKind::UnaryExpression)
        {
            const auto unaryExpression = (UnaryExpression*)node->rightExpression().get();
            if (unaryExpression->unaryOperator() == UnaryOperatorKind::ReferenceOf)
            {
                stream() << StringifyUnaryOperator(UnaryOperatorKind::ReferenceOf);
            }
        }
        stream() << " ";

        generateNode(node->leftExpression().get());
        stream() << " = ";
        generateNode(node->rightExpression().get());
        stream() << ";" << newLine();
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
                m_cppIncludes.append(include.value() % newLine());
            }
            stream() << indentation() << QString::fromStdString(std::string(typeName));
        }
        else
        {
            const auto type = node->type();
            const auto include = GetCppIncludeForType(type);
            if (include.has_value())
            {
                m_cppIncludes.append(include.value() % newLine());
            }
            stream() << indentation() << "auto";
        }

        // check if right expression is a ref
        if (node->rightExpression().has_value() && node->rightExpression().value()->kind() == NodeKind::UnaryExpression)
        {
            const auto unaryExpression = (UnaryExpression*)node->rightExpression().value().get();
            if (unaryExpression->unaryOperator() == UnaryOperatorKind::ReferenceOf)
            {
                stream() << StringifyUnaryOperator(UnaryOperatorKind::ReferenceOf);
            }
        }
        stream() << " ";

        generateNode(node->leftExpression().get());
        if (node->rightExpression().has_value())
        {
            stream() << " = ";
            generateNode(node->rightExpression().value().get());
        }
        stream() << ";" << newLine();
    }

    void CppCodeGenerator::generateTypeFieldDeclaration(TypeFieldDeclaration* node) noexcept
    {
        stream() << indentation();
        if (node->isConstant())
        {
            stream() << "const ";
        }

        if (node->explicitType().has_value())
        {
            const auto typeNameNode = node->explicitType().value().get();
            const auto typeName = getCppNameForType(typeNameNode);
            const auto include = GetCppIncludeForType(typeNameNode->type());
            if (include.has_value())
            {
                m_cppIncludes.append(include.value() % newLine());
            }
            stream() << QString::fromStdString(std::string(typeName)) << " ";
        }
        else
        {
            const auto type = node->type();
            const auto typeName = GetCppNameForType(node->type());
            const auto include = GetCppIncludeForType(type);
            if (include.has_value())
            {
                m_cppIncludes.append(include.value() % newLine());
            }
            stream() << QString::fromStdString(std::string(typeName)) << " ";
        }

        stream() << generateTypeFieldName(node->nameExpression().get());

        if (node->rightExpression().has_value())
        {
            if (node->rightExpression().value()->isLiteral())
            {
                stream() << " = ";
                generateNode(node->rightExpression().value().get());
            }
        }
        stream() << ";" << newLine();
    }

    void CppCodeGenerator::generateConstructorDeclarationSignature(std::string_view className, ParametersNode* parametersNode) noexcept
    {
        if(parametersNode == nullptr)
        {
            stream() << indentation() << QString::fromStdString(std::string(className)) << "() = default;" << newLine();
            return;
        }

        const auto isEmpty = parametersNode->parameters().empty();

        stream() << indentation() << QString::fromStdString(std::string(className)) << generateFunctionSignatureParameterPart(parametersNode);

        if (isEmpty)
        {
            stream() << " = default";
        }

        stream() << ";" << newLine();
    }

    void CppCodeGenerator::generateConstructorDefinition(CppTypeDef* cppType) noexcept
    {
        const auto& typeName = QString::fromStdString(std::string(cppType->name));
        const auto& parametersNode = cppType->constructorParameters;
        if (parametersNode == nullptr)
            return;

        const auto parameterPart = generateFunctionSignatureParameterPart(parametersNode);

        stream() << typeName << "::" << typeName << parameterPart << newLine();
        
        pushIndentation();

        QStringList initList;
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

                const auto initEntry = fieldName + "{ " + QString::fromStdString(std::string(parameterName)) + " }";

                initList.push_back(initEntry);
            }
        }

        if(!initList.isEmpty())
        {
            stream() << indentation() << ": " << initList.join(newLine() % ", ") << newLine();
        }

        popIndentation();
        stream() << "{" << newLine() << "}" << newLine() << newLine();
    }

    void CppCodeGenerator::generateDestructorDeclarationSignature(std::string_view className) noexcept
    {
        stream() << indentation() << "~" << QString::fromStdString(std::string(className)) << "() = default;" << newLine();
    }

    void CppCodeGenerator::generateMethodDeclarationSignature(MethodDefinitionStatement* node) noexcept
    {
        auto nameExpression = node->methodNameNode()->methodNameExpression().get();
        const auto methodName = m_parseTree.tokens().getLexeme(nameExpression->nameToken());
        auto parametersNode = node->parametersNode().get();
        auto returnTypesNode = node->returnTypesNode().get();
        const auto specialFunctionType = node->specialFunctionType();

        stream() << indentation();
        if (node->modifier() == MethodModifier::Static)
        {
            stream() << "static ";
        }

        stream() << generateFunctionSignatureReturnPart(returnTypesNode, false);
        stream() << QString::fromStdString(generateFunctionSignatureNamePart(methodName));
        stream() << generateFunctionSignatureParameterPart(parametersNode);

        if (specialFunctionType != SpecialFunctionType::None)
        {
            const auto& statements = node->bodyNode()->statements();
            if (statements.empty())
            {
                stream() << " = default";
            }
        }
        
        stream() << ";" << newLine();
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
        const auto signature = returnPart + QString::fromStdString(std::string(typeName)) + "::" + QString::fromStdString(namePart) + parameterPart;

        stream() << signature << newLine() << "{" << newLine();
        pushIndentation();
        const auto& body = node->bodyNode();
        for (const auto& statement : body->statements())
        {
            generateNode(statement.get());
        }
        popIndentation();
        stream() << "}" << newLine() << newLine();
    }

    void CppCodeGenerator::generateGlobalDiscardedExpression(Expression* expression) noexcept
    {
        stream() << indentation() << "const auto _" << m_discardCount++ << " = ";
        generateNode(expression);
        stream() << ";" << newLine();
    }

    void CppCodeGenerator::generateLocalDiscardedExpression(Expression* expression) noexcept
    {
        stream() << indentation() << "static_cast<void>(";
        generateNode(expression);
        stream() << ");" << newLine();
    }

    void CppCodeGenerator::generateAssignmentStatement(AssignmentStatement* node) noexcept
    {
        stream() << indentation();
        generateNode(node->leftExpression().get());
        stream() << " = ";
        generateNode(node->rightExpression().get());
        stream() << ";" << newLine();
    }

    QString CppCodeGenerator::generateEnumSignature(EnumDefinitionStatement* node) noexcept
    {
        const auto enumName = m_parseTree.tokens().getLexeme(node->nameExpression()->nameToken());

        QString signature;
        QTextStream sigStream(&signature);

        sigStream << indentation() << "enum class " << QString::fromStdString(std::string(enumName));
        if (node->baseType().has_value())
        {
            const auto baseType = node->baseType().value().get();
            const auto cppTypeName = GetCppNameForType(baseType->type());

            sigStream << " : " << QString::fromStdString(std::string(cppTypeName));
        }
        else
        {
            sigStream << " : unsigned char"; // 1 byte default size if not specified
        }

        return signature;
    }

    QString CppCodeGenerator::generateFunctionSignatureReturnPart(ReturnTypesNode* returnTypesNode, bool isMainFunction) noexcept
    {
        const auto& returnTypes = returnTypesNode->returnTypes();
        const auto hasReturnTypes = !returnTypes.empty();

        QString signature;
        QTextStream sigStream(&signature);

        if (!hasReturnTypes)
        {
            if (isMainFunction)
            {
                sigStream << "int ";
            }
            else
            {
                sigStream << "void ";
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
                m_cppIncludes.append(include.value() % newLine());
            }
            const auto returnTypeName = GetCppNameForType(returnType);
            sigStream << QString::fromStdString(std::string(returnTypeName)) << " ";
        }

        return signature;
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

    QString CppCodeGenerator::generateFunctionSignatureParameterPart(ParametersNode* parametersNode) noexcept
    {
        const auto& parameters = parametersNode->parameters();
        QString signature;
        QTextStream sigStream(&signature);

        sigStream << "(";
        for (const auto& parameter : parameters)
        {
            const auto typeName = getCppNameForType(parameter->typeName().get());
            if (parameter->typeName()->isReference())
            {
                sigStream << QString::fromStdString(std::string(typeName)) << "& ";
            }
            else
            {
                sigStream << QString::fromStdString(std::string(typeName)) << " ";
            }
            const auto& parameterNameToken = parameter->nameExpression()->nameToken();
            sigStream << QString::fromStdString(std::string(m_parseTree.tokens().getLexeme(parameterNameToken)));

            if (&parameter != &parameters.back())
            {
                sigStream << ", ";
            }
        }
        sigStream << ")";

        return signature;
    }

    void CppCodeGenerator::generateTypeDefinitionStatement(TypeDefinitionStatement* node) noexcept
    {
        auto cppTypeDef = buildCppTypeDefinition(node);
        auto typeName = cppTypeDef->name;

        const auto typeSignature = QString("class %2").arg(QString::fromStdString(std::string(typeName)));
        m_forwardDeclarations.append(typeSignature % ";" % newLine());

        stream() << indentation() << typeSignature << newLine();
        stream() << indentation() << "{" << newLine();
        pushIndentation();
        {
            const auto hasPublicMethods = (!cppTypeDef->publicMethods.empty() || !cppTypeDef->staticMethods.empty());
            const auto hasPrivateMethods = (!cppTypeDef->privateMethods.empty());
            const auto hasPublicFields = (!cppTypeDef->publicFields.empty());

            stream() << "public:" << newLine();

            generateConstructorDeclarationSignature(typeName, cppTypeDef->constructorParameters);
            generateDestructorDeclarationSignature(typeName);

            if (hasPublicMethods)
            {
                stream() << newLine();
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
                stream() << newLine() << "private:" << newLine();
                for (const auto& privateMethod : cppTypeDef->privateMethods)
                {
                    generateMethodDeclarationSignature(privateMethod);
                }
            }

            if (hasPublicFields)
            {
                if (hasPrivateMethods)
                {
                    stream() << newLine();
                }

                stream() << newLine() << "public:" << newLine();
                for (const auto& publicField : cppTypeDef->publicFields)
                {
                    generateTypeFieldDeclaration(publicField);
                }
            }
        }
        popIndentation();
        stream() << indentation() << "};" << newLine() << newLine();
    }

    QString CppCodeGenerator::generateTypeFieldName(NameExpression* node) noexcept
    {
        const auto fieldName = m_parseTree.tokens().getLexeme(node->nameToken());
        if (fieldName.starts_with('_'))
        {
            return "m" + QString::fromStdString(std::string(fieldName));
        }
        else
        {
            return "m_" + QString::fromStdString(std::string(fieldName));
        }
    }

    void CppCodeGenerator::generateEnumDefinitionStatement(EnumDefinitionStatement* node) noexcept
    {
        const auto signature = generateEnumSignature(node);
        m_forwardDeclarations.append(signature % ";" % newLine());
        stream() << indentation() << signature << newLine();

        stream() << indentation() << "{" << newLine();
        pushIndentation();
        for (const auto& fieldNode : node->fieldNodes())
        {
            stream() << indentation();
            generateNameExpression(fieldNode->nameExpression().get());
            if (fieldNode->valueExpression().has_value())
            {
                stream() << " = ";
                generateNode(fieldNode->valueExpression().value().get());
            }
            stream() << "," << newLine();
        }
        popIndentation();
        stream() << indentation() << "};" << newLine() << newLine();
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
        const auto signature = returnPart + QString::fromStdString(std::string(namePart)) + parameterPart;

        m_forwardDeclarations.append(signature % ";" % newLine());
        stream() << indentation() << signature << newLine() << indentation() << "{" << newLine();

        pushIndentation();
        const auto& body = node->bodyNode();
        for (const auto& statement : body->statements())
        {
            generateNode(statement.get());
        }
        popIndentation();

        stream() << indentation() << "}" << newLine() << newLine();

        m_currentScope = oldScope; // Reset the scope after generating the function definition
    }

    void CppCodeGenerator::generateIfStatement(IfStatement* node) noexcept
    {
        const auto& condition = node->condition();
        const auto needsParentheses = condition->kind() != NodeKind::GroupingExpression;

        // unwrap the condition if it is a grouping expression
        if (needsParentheses)
        {
            stream() << indentation() << "if (";
        }
        else
        {
            stream() << indentation() << "if ";
        }
        generateNode(node->condition().get());
        if (needsParentheses)
        {
            stream() << ")" << newLine();
        }
        else
        {
            stream() << newLine();
        }

        const auto trueBlockNeedsBrackets = node->trueStatement()->kind() != NodeKind::BlockNode;
        if (trueBlockNeedsBrackets)
        {
            stream() << indentation() << "{" << newLine();
            pushIndentation();
        }
        generateNode(node->trueStatement().get());
        if (trueBlockNeedsBrackets)
        {
            popIndentation();
            stream() << indentation() << "}" << newLine();
        }

        // Check if there is an else block
        if (node->hasFalseBlock())
        {
            const auto& falseStatement = node->falseStatement().value();
            const auto falseBlockNeedsBrackets = falseStatement->kind() != NodeKind::BlockNode;

            stream() << indentation() << "else" << newLine();
            if (falseBlockNeedsBrackets)
            {
                stream() << indentation() << "{" << newLine();
                pushIndentation();
            }
            generateNode(falseStatement.get());
            if (falseBlockNeedsBrackets)
            {
                popIndentation();
                stream() << indentation() << "}" << newLine();
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
            stream() << indentation() << "while (";
        }
        else
        {
            stream() << indentation() << "while ";
        }
        generateNode(node->condition().get());
        if (needsParentheses)
        {
            stream() << ")" << newLine();
        }
        else
        {
            stream() << newLine();
        }

        const auto bodyNeedsBrackets = node->trueStatement()->kind() != NodeKind::BlockNode;
        if (bodyNeedsBrackets)
        {
            stream() << indentation() << "{" << newLine();
            pushIndentation();
        }
        generateNode(node->trueStatement().get());
        if (bodyNeedsBrackets)
        {
            popIndentation();
            stream() << indentation() << "}" << newLine();
        }
    }

    void CppCodeGenerator::generateBreakStatement(BreakStatement* /*node*/) noexcept
    {
        stream() << indentation() << "break;" << newLine();
    }

    void CppCodeGenerator::generateSkipStatement(SkipStatement* /*node*/) noexcept
    {
        stream() << indentation() << "continue;" << newLine();
    }

    void CppCodeGenerator::generateReturnStatement(ReturnStatement* node) noexcept
    {
        stream() << indentation() << "return";
        if (node->expression().has_value())
        {
            stream() << " ";
            generateNode(node->expression().value().get());
        }
        stream() << ";" << newLine();
    }

    void CppCodeGenerator::generateGroupingExpression(GroupingExpression* node) noexcept
    {
        stream() << "(";
        generateNode(node->expression().get());
        stream() << ")";
    }

    void CppCodeGenerator::generateUnaryExpression(UnaryExpression* node) noexcept
    {
        if (node->unaryOperator() != UnaryOperatorKind::ReferenceOf)
        {
            const auto unaryOperator = StringifyUnaryOperator(node->unaryOperator());
            stream() << unaryOperator;
        }
        generateNode(node->expression().get());
    }

    void CppCodeGenerator::generateBinaryExpression(BinaryExpression* node) noexcept
    {
        const auto binaryOperator = node->binaryOperator();
        if (binaryOperator == BinaryOperatorKind::MemberAccess)
        {
            generateNode(node->leftExpression().get());
            stream() << "::";
            generateNode(node->rightExpression().get());
        }
        else
        {
            const auto binaryOperatorString = StringifyBinaryOperator(node->binaryOperator());

            generateNode(node->leftExpression().get());
            stream() << " " << binaryOperatorString << " ";
            generateNode(node->rightExpression().get());
        }
    }

    void CppCodeGenerator::generateNameExpression(NameExpression* node) noexcept
    {
        stream() << QString::fromStdString(std::string(m_parseTree.tokens().getLexeme(node->nameToken())));
    }

    void CppCodeGenerator::generateFunctionCallExpression(FunctionCallExpression* node) noexcept
    {
        generateNode(node->nameExpression().get());
        stream() << "(";
        const auto& arguments = node->argumentsNode()->arguments();
        for (const auto& argument : arguments)
        {
            generateNode(argument.get());

            if (&argument != &arguments.back())
            {
                stream() << ", ";
            }
        }
        stream() << ")";
    }

    void CppCodeGenerator::generateMemberAccessExpression(MemberAccessExpression* node) noexcept
    {
        auto expression = node->expression().get();
        stream() << "this->";
        if (expression->kind() == NodeKind::FunctionCallExpression)
        {
            generateFunctionCallExpression((FunctionCallExpression*)expression);
        }
        else
        {
            stream() << generateTypeFieldName((NameExpression*)expression);
        }
    }

    void CppCodeGenerator::generateBoolLiteral(BoolLiteral* node) noexcept
    {
        auto value = node->value() ? QStringLiteral("true") : QStringLiteral("false");
        stream() << value;
    }

    void CppCodeGenerator::generateNumberLiteral(NumberLiteral* node) noexcept
    {
        auto lexeme = m_parseTree.tokens().getLexeme(node->literalToken());
        stream() << QString::fromStdString(std::string(lexeme));
    }

    void CppCodeGenerator::generateStringLiteral(StringLiteral* node) noexcept
    {
        auto lexeme = m_parseTree.tokens().getLexeme(node->literalToken());
        stream() << QString("std::string{%1}").arg(QString::fromStdString(std::string(lexeme)));
    }

    void CppCodeGenerator::generateBuiltinPrintFunction(ArgumentsNode* node) noexcept
    {
        m_cppIncludes.append(QStringLiteral("#include <iostream>") % newLine());

        if (m_currentStatement == NodeKind::ExpressionStatement)
        {
            stream() << "std::cout << ";
        }
        else
        {
            stream() << "&(std::cout << ";
        }

        const auto& arguments = node->arguments();

        for (const auto& argument : arguments)
        {
            const auto type = argument->type();
            const auto include = GetCppIncludeForType(type);
            if (include.has_value())
            {
                m_cppIncludes.append(include.value() % newLine());
            }

            generateNode(argument.get());

            if (&argument != &arguments.back())
            {
                stream() << " << ";
            }
        }

        if (m_currentStatement == NodeKind::ExpressionStatement)
        {
            stream() << " << \"\\n\"";
        }
        else
        {
            stream() << " << \"\\n\")";
        }
    }

    QString generateCpp(ParseTree& parseTree) noexcept
    {
        CppCodeGenerator generator{ parseTree };
        return generator.generate();
    }
}
