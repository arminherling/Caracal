#include <CodeGen/CppCodeGenerator.h>

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

    CppCodeGenerator::CppCodeGenerator(ParseTree& parseTree, i32 indentation)
        : BasePrinter(indentation)
        , m_parseTree{ parseTree }
    {
    }

    QString CppCodeGenerator::generate()
    {
        for (const auto& statement : m_parseTree.statements())
        {
            generateNode(statement.get());
        }

        return toUtf8();
    }

    void CppCodeGenerator::generateNode(Node* node)
    {
        switch (node->kind())
        {
            case NodeKind::CppBlockStatement:
            {
                generateCppBlock((CppBlockStatement*)node);
                break;
            }
            case NodeKind::FunctionDefinitionStatement:
            {
                generateFunctionDefinition((FunctionDefinitionStatement*)node);
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

    void CppCodeGenerator::generateFunctionDefinition(FunctionDefinitionStatement* node)
    {
        const auto hasReturnTypes = node->returnTypes()->returnTypes().empty() == false;
        const auto functionName = m_parseTree.tokens().getLexeme(node->name());
        const auto isMainFunction = functionName == QStringLiteral("main");
        if (!hasReturnTypes && isMainFunction)
        {
            stream() << indentation() << "int";
        }
        else
        {
            TODO("Implement return types in CppCodeGenerator");
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
        stream() << indentation() << "}" << newLine();
    }

    QString generateCpp(ParseTree& parseTree) noexcept
    {
        CppCodeGenerator generator{ parseTree };
        return generator.generate();
    }
}
