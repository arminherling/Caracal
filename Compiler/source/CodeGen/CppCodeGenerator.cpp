#include <CodeGen/CppCodeGenerator.h>
#include <QStringBuilder>

namespace Caracal
{
    static QString ReplaceEscapeSequences(QStringView input)
    {
        QString result = input.toString();
        result.replace(QLatin1StringView("\\\'"), QLatin1StringView("\'"));
        result.replace(QLatin1StringView("\\\""), QLatin1StringView("\""));
        result.replace(QLatin1StringView("\\a"), QLatin1StringView("\a"));
        result.replace(QLatin1StringView("\\b"), QLatin1StringView("\b"));
        result.replace(QLatin1StringView("\\f"), QLatin1StringView("\f"));
        result.replace(QLatin1StringView("\\n"), QLatin1StringView("\n"));
        result.replace(QLatin1StringView("\\r"), QLatin1StringView("\r"));
        result.replace(QLatin1StringView("\\t"), QLatin1StringView("\t"));
        result.replace(QLatin1StringView("\\v"), QLatin1StringView("\v"));
        result.replace(QLatin1StringView("\\\\"), QLatin1StringView("\\"));
        return result;
    }

    [[nodiscard]] static auto InitializeTypeToCppName() noexcept
    {
        return std::unordered_map<Type, QStringView>{
            { Type::Bool(), QStringView(u"bool") },
            { Type::I32(), QStringView(u"int") },
            { Type::F32(), QStringView(u"float") },
            { Type::String(), QStringView(u"std::string") },
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

        // collect fields
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
                if (specialFunctionType == SpecialFunctionType::Constructor)
                {
                    cppTypeDef->constructors.emplace_back(methodDefinition);
                }
                else if (specialFunctionType == SpecialFunctionType::Destructor)
                {
                    cppTypeDef->destructor = methodDefinition;
                }
            }
        }

        m_cppTypeDefs.insert({ typeName, std::move(cppTypeDef) });
        return m_cppTypeDefs[typeName].get();
    }

    [[nodiscard]] QStringView CppCodeGenerator::getCppNameForType(TypeNameNode* typeName) noexcept
    {
        static const auto cppTypeNames = InitializeTypeToCppName();
        if (const auto result = cppTypeNames.find(typeName->type()); result != cppTypeNames.end())
            return result->second;

        return m_parseTree.tokens().getLexeme(typeName->name()->nameToken());
    }

    [[nodiscard]] static QStringView GetCppNameForType(Type type) noexcept
    {
        static const auto cppTypeNames = InitializeTypeToCppName();
        if (const auto result = cppTypeNames.find(type); result != cppTypeNames.end())
            return result->second;

        return QStringView(u"");
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
        if (!m_cppTypeDefs.empty())
            stream() << newLine();
        // generate cpp method definitions
        for (const auto& [typeName, cppTypeDef] : m_cppTypeDefs)
        {
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
                if (m_parseTree.tokens().getLexeme(nameToken) == QStringLiteral("print"))
                {
                    generateBuiltinPrintFunction(functionCallExpression->argumentsNode().get());
                    break;
                }

                generateFunctionCallExpression(functionCallExpression);
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
            lexeme = lexeme.mid(1, lexeme.length() - 2);
            const auto result = ReplaceEscapeSequences(lexeme);
            stream() << indentation() << result << newLine();
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
            stream() << indentation() << "const " << typeName;
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

            if (cppTypeName.isEmpty())
            {
                stream() << indentation() << "const auto";
            }
            else
            {
                stream() << indentation() << "const " << cppTypeName;
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
            stream() << indentation() << typeName;
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
            stream() << typeName;
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
            stream() << typeName;
        }
        stream() << " ";
        generateNameExpression(node->nameExpression().get());

        if (node->rightExpression().has_value())
        {
            stream() << " = ";
            generateNode(node->rightExpression().value().get());
        }
        stream() << ";" << newLine();
    }

    void CppCodeGenerator::generateMethodDeclaration(MethodDefinitionStatement* node) noexcept
    {
        stream() << indentation();
        auto methodName = node->methodNameNode()->methodNameExpression().get();
        auto parametersNode = node->parametersNode().get();
        auto returnTypesNode = node->returnTypesNode().get();
        const auto signature = generateFunctionSignature(std::nullopt, methodName, parametersNode, returnTypesNode);
        if (node->modifier() == MethodModifier::Static)
        {
            stream() << "static ";
        }

        stream() << signature << ";" << newLine();
    }

    void CppCodeGenerator::generateMethodDefinition(const QStringView& typeName, MethodDefinitionStatement* node) noexcept
    {
        auto methodName = node->methodNameNode()->methodNameExpression().get();
        auto parametersNode = node->parametersNode().get();
        auto returnTypesNode = node->returnTypesNode().get();
        const auto signature = generateFunctionSignature(typeName, methodName, parametersNode, returnTypesNode);
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

        sigStream << indentation() << "enum class " << enumName;
        if (node->baseType().has_value())
        {
            const auto baseType = node->baseType().value().get();
            const auto cppTypeName = GetCppNameForType(baseType->type());

            sigStream << " : " << cppTypeName;
        }
        else
        {
            sigStream << " : unsigned char"; // 1 byte default size if not specified
        }

        return signature;
    }

    QString CppCodeGenerator::generateFunctionSignature(std::optional<QStringView> className, NameExpression* nameExpression, ParametersNode* parametersNode, ReturnTypesNode* returnTypesNode) noexcept
    {
        const auto& returnTypes = returnTypesNode->returnTypes();
        const auto hasReturnTypes = !returnTypes.empty();
        const auto functionName = m_parseTree.tokens().getLexeme(nameExpression->nameToken());
        const auto isMainFunction = functionName == QStringLiteral("main");

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
            sigStream << returnTypeName << " ";
        }

        if (className.has_value())
        {
            sigStream << className.value() << "::";
        }

        if (functionName.startsWith('_'))
        {
            // remove first letter
            sigStream << functionName.mid(1) << "(";
        }
        else
        {
            sigStream << functionName << "(";
        }

        const auto& parameters = parametersNode->parameters();
        for (const auto& parameter : parameters)
        {
            const auto typeName = getCppNameForType(parameter->typeName().get());
            if (parameter->typeName()->isReference())
            {
                sigStream << typeName << "& ";
            }
            else
            {
                sigStream << typeName << " ";
            }
            const auto& parameterNameToken = parameter->nameExpression()->nameToken();
            sigStream << m_parseTree.tokens().getLexeme(parameterNameToken);

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

        const auto typeSignature = QString("class %2").arg(typeName);
        m_forwardDeclarations.append(typeSignature % ";" % newLine());

        stream() << indentation() << typeSignature << newLine();
        stream() << indentation() << "{" << newLine();
        pushIndentation();
        {
            if (!cppTypeDef->constructors.empty() || !cppTypeDef->staticMethods.empty() || !cppTypeDef->publicMethods.empty())
            {
                stream() << "public:" << newLine();
                for (const auto& staticMethod : cppTypeDef->staticMethods)
                {
                    generateMethodDeclaration(staticMethod);
                }
                for (const auto& publicMethod : cppTypeDef->publicMethods)
                {
                    generateMethodDeclaration(publicMethod);
                }
            }

            if (!cppTypeDef->privateMethods.empty())
            {
                stream() << "private:" << newLine();
                for (const auto& privateMethod : cppTypeDef->privateMethods)
                {
                    generateMethodDeclaration(privateMethod);
                }
            }

            if (!cppTypeDef->publicFields.empty())
            {
                stream() << "public:" << newLine();
            }
            for (const auto& publicField : cppTypeDef->publicFields)
            {
                generateTypeFieldDeclaration(publicField);
            }
        }
        popIndentation();
        stream() << indentation() << "};" << newLine();
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
        auto parametersNode = node->parametersNode().get();
        auto returnTypesNode = node->returnTypesNode().get();

        const auto signature = generateFunctionSignature(std::nullopt, nameExpression, parametersNode, returnTypesNode);
        m_forwardDeclarations.append(signature % ";" % newLine());
        stream() << indentation() << signature << newLine();
        stream() << indentation() << "{" << newLine();

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
        stream() << m_parseTree.tokens().getLexeme(node->nameToken());
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

    void CppCodeGenerator::generateBoolLiteral(BoolLiteral* node) noexcept
    {
        auto value = node->value() ? QStringLiteral("true") : QStringLiteral("false");
        stream() << value;
    }

    void CppCodeGenerator::generateNumberLiteral(NumberLiteral* node) noexcept
    {
        auto lexeme = m_parseTree.tokens().getLexeme(node->literalToken());
        stream() << lexeme;
    }

    void CppCodeGenerator::generateStringLiteral(StringLiteral* node) noexcept
    {
        auto lexeme = m_parseTree.tokens().getLexeme(node->literalToken());
        stream() << QString("std::string{%1}").arg(lexeme);
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
