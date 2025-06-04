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

    [[nodiscard]] static QStringView GetCppNameForType(Type type) noexcept
    {
        static const auto cppTypeNames = InitializeTypeToCppName();
        if (const auto result = cppTypeNames.find(type); result != cppTypeNames.end())
            return result->second;
        
        TODO("Type not found in GetCppNameForType");
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
            default:
                TODO("Implement StringifyBinaryOperator for all operators");
                return QString();
        }
    }

    CppCodeGenerator::CppCodeGenerator(ParseTree& parseTree, i32 indentation)
        : BasePrinter(indentation)
        , m_parseTree{ parseTree }
        , m_cppIncludes{ }
        , m_currentScope(Scope::Global)
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

        return includes % toUtf8();
    }

    void CppCodeGenerator::generateNode(Node* node)
    {
        switch (node->kind())
        {
            case NodeKind::ConstantDeclaration:
            {
                generateConstantDeclaration((ConstantDeclaration*)node);
                break;
            }
            case NodeKind::VariableDeclaration:
            {
                generateVariableDeclaration((VariableDeclaration*)node);
                break;
            }
            case NodeKind::CppBlockStatement:
            {
                generateCppBlock((CppBlockStatement*)node);
                break;
            }
            case NodeKind::AssignmentStatement:
            {
                generateAssignmentStatement((AssignmentStatement*)node);
                break;
            }
            case NodeKind::FunctionDefinitionStatement:
            {
                generateFunctionDefinition((FunctionDefinitionStatement*)node);
                break;
            }
            case NodeKind::ReturnStatement:
            {
                generateReturnStatement((ReturnStatement*)node);
                break;
            }
            case NodeKind::BinaryExpression:
            {
                generateBinaryExpression((BinaryExpression*)node);
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

            default:
            {
                stream() << indentation() << QString("Missing NodeKind!!") << newLine();
                break;
            }
        }
    }

    void CppCodeGenerator::generateCppBlock(CppBlockStatement* node)
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

    void CppCodeGenerator::generateConstantDeclaration(ConstantDeclaration* node)
    {
        const auto& identifierToken = node->identifier();
        const auto isDiscard = identifierToken.kind == TokenKind::Underscore;
        if (isDiscard)
        {
            if (m_currentScope == Scope::Function)
            {
                stream() << indentation() << "static_cast<void>(";
                generateNode(node->expression().get());
                stream() << ");" << newLine();
            }
            else
            {
                stream() << indentation() << "constexpr auto _ = ";
                generateNode(node->expression().get());
                stream() << ";" << newLine();
            }
        }
        else
        {
            const auto& identifier = m_parseTree.tokens().getLexeme(node->identifier());
            const auto type = node->expression()->type();
            const auto include = GetCppIncludeForType(type);
            if (include.has_value())
            {
                m_cppIncludes.append(include.value() % newLine());
            }
            stream() << indentation() << "constexpr auto " << identifier << " = ";
            generateNode(node->expression().get());
            stream() << ";" << newLine();
        }
    }

    void CppCodeGenerator::generateVariableDeclaration(VariableDeclaration* node)
    {
        const auto& identifierToken = node->identifier();
        const auto isDiscard = identifierToken.kind == TokenKind::Underscore;
        if (isDiscard)
        {
            stream() << indentation() << "static_cast<void>(";
            generateNode(node->expression().get());
            stream() << ");" << newLine();
        }
        else
        {
            const auto& identifier = m_parseTree.tokens().getLexeme(node->identifier());
            const auto type = node->expression()->type();
            const auto include = GetCppIncludeForType(type);
            if (include.has_value())
            {
                m_cppIncludes.append(include.value() % newLine());
            }
            stream() << indentation() << "auto " << identifier << " = ";
        generateNode(node->expression().get());
        stream() << ";" << newLine();
        }
    }

    void CppCodeGenerator::generateAssignmentStatement(AssignmentStatement* node)
    {
        const auto& identifier = m_parseTree.tokens().getLexeme(node->identifier());
        const auto type = node->expression()->type();
        
        stream() << indentation() << identifier << " = ";
        generateNode(node->expression().get());
        stream() << ";" << newLine();
    }

    void CppCodeGenerator::generateFunctionDefinition(FunctionDefinitionStatement* node)
    {
        const auto oldScope = m_currentScope;
        m_currentScope = Scope::Function;

        const auto& returnTypes = node->returnTypes()->returnTypes();
        const auto hasReturnTypes = returnTypes.empty() == false;
        const auto functionName = m_parseTree.tokens().getLexeme(node->name());
        const auto isMainFunction = functionName == QStringLiteral("main");
        if (!hasReturnTypes)
        {
            if (isMainFunction)
            {
                stream() << indentation() << "int";
            }
            else
            {
                stream() << indentation() << "void";
            }
        }
        else
        {
            if (returnTypes.size() != 1)
            {
                TODO("Implement multiple return types in CppCodeGenerator");
            }

            const auto type = returnTypes[0]->type();
            const auto include = GetCppIncludeForType(type);
            if (include.has_value())
            {
                m_cppIncludes.append(include.value() % newLine());
            }

            const auto typeName = GetCppNameForType(type);
            stream() << indentation() << typeName;
        }
        stream() << indentation() << " " << functionName;

        const auto hasParameters = node->parameters()->parameters().empty() == false;
        if (!hasParameters)
        {
            stream() << indentation() << "()" << newLine();
        }
        else
        {
            TODO("Implement parameters in CppCodeGenerator");
        }

        const auto& body = node->body();
        stream() << indentation() << "{" << newLine();
        pushIndentation();

        for (const auto& statement : body->statements())
        {
            generateNode(statement.get());
        }

        popIndentation();
        stream() << indentation() << "}" << newLine() << newLine();

        m_currentScope = oldScope; // Reset the scope after generating the function definition
    }

    void CppCodeGenerator::generateReturnStatement(ReturnStatement* node)
    {
        stream() << indentation() << "return";
        if (node->expression().has_value())
        {
            stream() << " ";
            generateNode(node->expression().value().get());
        }
        stream() << ";" << newLine();
    }

    void CppCodeGenerator::generateBinaryExpression(BinaryExpression* node)
    {
        const auto binaryOperator = StringifyBinaryOperator(node->binaryOperator());
        
        generateNode(node->leftExpression().get());
        stream() << " " << binaryOperator << " ";
        generateNode(node->rightExpression().get());
    }

    void CppCodeGenerator::generateBoolLiteral(BoolLiteral* node)
    {
        auto value = node->value() ? QStringLiteral("true") : QStringLiteral("false");
        stream() << value;
    }

    void CppCodeGenerator::generateNumberLiteral(NumberLiteral* node)
    {
        auto lexeme = m_parseTree.tokens().getLexeme(node->token());
        stream() << lexeme;
    }

    void CppCodeGenerator::generateStringLiteral(StringLiteral* node)
    {
        auto lexeme = m_parseTree.tokens().getLexeme(node->token());
        stream() <<  QString("std::string{%1}").arg(lexeme);
    }

    QString generateCpp(ParseTree& parseTree) noexcept
    {
        CppCodeGenerator generator{ parseTree };
        return generator.generate();
    }
}
